#include "ResourceManager.h"
#include <fstream>
#include "ResourceLoader.h"
#include "loader/DDSLoader.h"
#include "loader/ModelLoader.h"
#include "loader/TextureLoader.h"
#include <utility/Logger.h>
#include "ModelBank.h"
// glcontext handling
#include <utility/PlatformDefinitions.h>

#if PLATFORM == PLATFORM_WINDOWS
	#include "SDL_syswm.h"
	#include <GL/wglew.h>
	HGLRC LoadingContext;
	HGLRC MainContext;
	HDC	  Device;
#elif PLATFORM == PLATFORM_LINUX
	#include <GL/glxew.h>
	#include "SDL_syswm.h"
	typedef GLXContext (*glXCreateContextAttribsARBProc)( Display*, GLXFBConfig, GLXContext, Bool, const int* );
	// GLXContext g_MainContext;
	// GLXContext g_LoadingContext;
	// Display* g_Display;
	// Window g_Window;
	SDL_GLContext g_SDLLoadContext;
	// SDL_Window* g_MainWindow;
#endif

ResourceManager& ResourceManager::GetInstance() {
	static ResourceManager resourceManager;

	return resourceManager;
}

ResourceManager::ResourceManager() {
	m_MemoryUsage = 0;
	// LSR new for all resource loaders
	AddResourceLoader( std::make_unique<DDSLoader>(),	  { "dds" } );
	AddResourceLoader( std::make_unique<ModelLoader>(),	  { "obj", "dae" } );
	AddResourceLoader( std::make_unique<TextureLoader>(), { "png", "tga" } );
}

ResourceManager::~ResourceManager() {
	m_ResourceLoaders.clear();
	m_ResourceLoaderMapping.clear();
	UnloadAllResources();
}

void ResourceManager::WorkerThread( SDL_Window* window ) {
#if PLATFORM == PLATFORM_WINDOWS
		wglMakeCurrent( Device, LoadingContext );
#elif PLATFORM == PLATFORM_LINUX
		// glXMakeCurrent( g_Display, g_Window, g_LoadingContext );
		SDL_GL_MakeCurrent( window, g_SDLLoadContext );
#endif
	while ( true ) {
		// lock mutex for queue
		m_JobQueueMutex.lock();
		if ( m_JobQueue.size() > 0 ) {
			ResourceJob job = m_JobQueue.front();
			m_JobQueue.pop();
			m_JobQueueMutex.unlock();
			if ( job.File.ID == WORKER_THREAD_STOP ) {
				return;
			}
			m_ResourceLoaderMutex.lock_shared();
			auto loaderIterator = m_ResourceLoaderMapping.find( job.File.Suffix );
			if ( loaderIterator != m_ResourceLoaderMapping.end() ) {
				Logger::Log( "Loaded resource with suffix: " + job.File.Suffix, "ResourceManager", LogSeverity::DEBUG_MSG ); // TODOJM: Reverse lookup name
				job.Entry->ReferenceCount = 0;
				job.Entry->Resource		  = loaderIterator->second->LoadResource( job.File );
				m_ResourceLoaderMutex.unlock_shared();
				if (job.Entry->Resource) {
					m_MemoryUsage += job.Entry->Resource->GetSize();
					m_ResourceMutex.lock();
					if (m_MemoryUsage > MAX_MEMORY_USAGE) {
						EvictUntilEnoughMemory();
					}
					auto it = m_Resources.emplace(job.File.ID, std::move(*job.Entry));
					m_ResourceMutex.unlock();
				}
			} else {
				m_ResourceLoaderMutex.unlock_shared();
				Logger::Log( "Failed to find resource loader for suffix: " + job.File.Suffix, "ResourceManager", LogSeverity::ERROR_MSG );
				return;
			}
		} else {
			m_JobQueueMutex.unlock();
		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
}

void ResourceManager::StartWorkerThread( SDL_Window* window, SDL_GLContext mainContext ) {
#if PLATFORM == PLATFORM_WINDOWS
		SDL_SysWMinfo info;
		SDL_VERSION( &info.version );
		if ( SDL_GetWindowWMInfo( window, &info ) < 0 ) {
			assert( false );
		}
		HWND hWnd = info.info.win.window;
		Device		   = GetDC( hWnd );
		MainContext	   = wglGetCurrentContext();
		LoadingContext = wglCreateContext( Device );
		wglShareLists( MainContext, LoadingContext );
#endif
#if PLATFORM == PLATFORM_LINUX
		SDL_SysWMinfo info;
		SDL_VERSION( &info.version );
		if ( SDL_GetWindowWMInfo( window, &info ) < 0 ) {
			assert( false );
		}
		SDL_GL_MakeCurrent( window, mainContext );
		SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );
		g_SDLLoadContext = SDL_GL_CreateContext( window );
		SDL_GL_MakeCurrent( window, mainContext );
#endif

	m_WorkerThread = std::thread( &ResourceManager::WorkerThread, this, window );
	m_WorkerThread.detach();
}

void ResourceManager::PostQuitJob() {
	ResourceJob job;
	FileContent fc;

	fc.ID	 = WORKER_THREAD_STOP;
	job.File = fc;
	m_JobQueueMutex.lock();
	m_JobQueue.push( job );
	m_JobQueueMutex.unlock();
}

void ResourceManager::UnloadAllResources() {
	for ( auto& resource : m_Resources ) {
		Logger::Log( "Resource: " + rToString( resource.first ) + " is still loaded, please release it properly", "ResourceManager", LogSeverity::WARNING_MSG );
		if ( resource.second.Resource ) {
			// ReleaseResource( resource.first );
		}
	}
	m_Resources.clear();
}

void ResourceManager::AquireResource( const ResourceIdentifier identifier ) {
	m_ResourceMutex.lock_shared();
	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator == m_Resources.end() ) {
		m_ResourceMutex.unlock_shared();
		m_PackageMutex.lock();
		FileContent fileContent = m_PackageManager.GetFileContent( identifier );
		m_PackageMutex.unlock();
		if ( fileContent.Loaded ) {
			// create job
			ResourceJob job;
			job.Entry			= new ResourceEntry();
			job.Entry->Resource = new Resource();
			job.File			= fileContent;

			m_JobQueueMutex.lock();
			m_JobQueue.push( job );
			m_JobQueueMutex.unlock();

			return;
		} else {
			return;
		}
	} else {
		resourceIterator->second.ReferenceCount++;
		m_ResourceMutex.unlock_shared();
		return;
	}
}

void ResourceManager::ReleaseResource( const ResourceIdentifier identifier ) {
	std::unique_lock<std::shared_timed_mutex>( m_ResourceMutex );
	auto resourceIterator = m_Resources.find( identifier );

	if ( resourceIterator != m_Resources.end() ) {
		resourceIterator->second.ReferenceCount--;
	} else {
		Logger::Log( "Tried to release a resource that was not loaded", "ResourceManager", LogSeverity::WARNING_MSG );
	}
}

Resource* ResourceManager::GetResourcePointer( const ResourceIdentifier identifier ) {
	std::shared_lock<std::shared_timed_mutex>( m_ResourceMutex );
	auto resourceIterator = m_Resources.find( identifier );

	if ( resourceIterator != m_Resources.end() ) {
		return resourceIterator->second.Resource;
	} else {
		return nullptr;
	}
}

void ResourceManager::AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader, std::initializer_list<pString> fileSuffixes ) {
	std::unique_lock<std::shared_timed_mutex>( m_ResourceLoaderMutex );
	m_ResourceLoaders.push_back( std::move( resourceLoader ) );
	for ( const auto& suffix : fileSuffixes ) {
		auto it = m_ResourceLoaderMapping.find( suffix );
		if ( it == m_ResourceLoaderMapping.end() ) {
			m_ResourceLoaderMapping.emplace( suffix, m_ResourceLoaders.back().get() );
		} else {
			Logger::Log( "Suffix: " + suffix + " mapping already exist.", "ResourceManager", LogSeverity::ERROR_MSG );
		}
	}
}

size_t ResourceManager::GetTotalResourceSize() const {
	return m_MemoryUsage;
}

void ResourceManager::EvictUntilEnoughMemory() {
    for ( auto resourceIt = m_Resources.begin(); resourceIt != m_Resources.end() && m_MemoryUsage > MAX_MEMORY_USAGE; ) {
		if ( resourceIt->second.ReferenceCount == 0 ) {
			m_MemoryUsage -= resourceIt->second.Resource->GetSize();
			delete resourceIt->second.Resource; // TODOJM: Use correct delete
            resourceIt = m_Resources.erase( resourceIt );
        } else {
            resourceIt++;
        }
	}
	if ( m_MemoryUsage > MAX_MEMORY_USAGE ) { // Could not evict enough
		Logger::Log( "Out of memory for resource manager. Dumping list of resources...", "ResourceManager", LogSeverity::ERROR_MSG );
		std::ofstream dumpFile( "resourceDump.txt" );
		if ( dumpFile.is_open() ) {
			for ( const auto& resource : m_Resources ) {
				dumpFile << resource.second.ReferenceCount << " " << resource.first << std::endl; // TODOJM: Reverse lookup
			}
		}
		dumpFile.close();
		// TODOJM: Log all shit to file
		assert( false );
	}
}
