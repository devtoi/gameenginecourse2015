#include "ResourceManager.h"
#include <fstream>
#include "ResourceLoader.h"
#include "loader/DDSLoader.h"
#include "loader/ModelLoader.h"
#include "loader/TextureLoader.h"
#include <utility/Logger.h>
#include <memory/DrinQStackAllocator.h>
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
	m_Allocator = new DrinQStackAllocator( STACK_ALLOCATOR_SIZE );
	m_PackageManager = new PackageManager( m_Allocator );

	m_MemoryUsage = 0;
	// LSR new for all resource loaders
	AddResourceLoader( std::make_unique<DDSLoader>(),	  { "dds" } );
	AddResourceLoader( std::make_unique<ModelLoader>(),	  { "obj", "dae" } );
	AddResourceLoader( std::make_unique<TextureLoader>(), { "png", "tga" } );

	m_LoaderThread = m_ThreadPool.CreateThread( ThreadType::Dedicated, "Loader thread" );
}

ResourceManager::~ResourceManager() {
	m_ResourceLoaders.clear();
	m_ResourceLoaderMapping.clear();
	UnloadAllResources();
	m_ThreadPool.Shutdown();
	delete m_Allocator;
}

bool ResourceManager::MakeContextCurrent( SDL_Window* window ) {
#if PLATFORM == PLATFORM_WINDOWS
		wglMakeCurrent( Device, LoadingContext );
#elif PLATFORM == PLATFORM_LINUX
		// glXMakeCurrent( g_Display, g_Window, g_LoadingContext );
		SDL_GL_MakeCurrent( window, g_SDLLoadContext );
#endif
	return true;
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
	std::future<bool> contextJob = m_ThreadPool.EnqueueJob( "Set GL context", m_LoaderThread,
		std::function<bool( SDL_Window* )>( std::bind( &ResourceManager::MakeContextCurrent, this, std::placeholders::_1 ) ), window );
	contextJob.get();

	// m_WorkerThread = std::thread( &ResourceManager::WorkerThread, this, window );
	// m_WorkerThread.detach();
}

void ResourceManager::UnloadAllResources() {
	std::unique_lock<std::shared_timed_mutex>( m_ResourceMutex );

	for ( auto& resource : m_Resources ) {
		if ( resource.second.ReferenceCount > 0 ) {
			Logger::Log( "Resource: " + rToString( resource.first ) + " is still referenced " +
				rToString( resource.second.ReferenceCount ) + " times, please release it properly",
				"ResourceManager", LogSeverity::WARNING_MSG );
		}
		if ( resource.second.Resource ) {
			delete resource.second.Resource;
			// ReleaseResource( resource.first );
		}
	}
	m_Resources.clear();
}

std::future<Resource*> ResourceManager::AquireResource( const ResourceIdentifier identifier ) {
	m_ResourceMutex.lock_shared();
	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator == m_Resources.end() ) {
		m_ResourceMutex.unlock_shared();
		// m_ResourcesBeingLoadedMutex.lock();
		return m_ThreadPool.EnqueueJob( "Load resource", m_LoaderThread, std::function<Resource*( ResourceIdentifier )>(
				std::bind( &ResourceManager::LoadResource, this, std::placeholders::_1 ) ), identifier );
		// } else {
		// std::packaged_task<Resource*( )> task([] () {
		// return nullptr;
		// } );
		// return task.get_future();
		// }
	} else {
		resourceIterator->second.ReferenceCount++;
		if ( resourceIterator->second.ReferenceCount == 1 ) {
			resourceIterator->second.Resource->ReaddDependencies();
		}

		std::packaged_task<Resource*( Resource* )> task([] ( Resource* resource ) {
			return resource;
		} );
		std::future<Resource*> future = task.get_future();
		task( resourceIterator->second.Resource );
		m_ResourceMutex.unlock_shared();
		return future;
	}
}

void ResourceManager::ReleaseResource( const ResourceIdentifier identifier ) {
	m_ResourceMutex.lock();
	auto resourceIterator = m_Resources.find( identifier );

	if ( resourceIterator != m_Resources.end() ) {
		resourceIterator->second.ReferenceCount--;
		if ( resourceIterator->second.ReferenceCount == 0 ) {
			m_ResourceMutex.unlock();
			resourceIterator->second.Resource->ReleaseDependencies();
		} else {
			m_ResourceMutex.unlock();
		}
	} else {
		m_ResourceMutex.unlock();
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

Resource* ResourceManager::LoadResource( const ResourceIdentifier identifier ) {
	m_PackageMutex.lock();
	size_t marker = m_Allocator->GetMarker();
	FileContent fileContent = m_PackageManager->GetFileContent( identifier, m_Allocator );
	m_PackageMutex.unlock();

	m_ResourceMutex.lock_shared();
	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator == m_Resources.end() ) {
		m_ResourceMutex.unlock_shared(); 
		m_ResourceLoaderMutex.lock_shared();
		auto loaderIterator = m_ResourceLoaderMapping.find( fileContent.Suffix );
		if ( loaderIterator != m_ResourceLoaderMapping.end() ) {
			ResourceEntry resourceEntry;
			resourceEntry.Resource = loaderIterator->second->LoadResource( fileContent );
			m_ResourceLoaderMutex.unlock_shared();
			if ( resourceEntry.Resource ) {
				Logger::Log( "Loaded resource " + LookupResourceName( identifier ) + " with suffix: " + fileContent.Suffix, "ResourceManager", LogSeverity::DEBUG_MSG ); // TODOJM: Reverse lookup name
				m_Allocator->Unwind( marker );
				m_MemoryUsage += resourceEntry.Resource->GetSize();
				std::unique_lock<std::shared_timed_mutex>( m_ResourceMutex );
				if ( m_MemoryUsage > MAX_MEMORY_USAGE ) {
					EvictUntilEnoughMemory();
				}
				auto it = m_Resources.emplace( identifier, std::move( resourceEntry ) );
				return it.first->second.Resource;
			} else {
				Logger::Log( "Failed to load resource " + LookupResourceName( identifier ) + " with suffix: " + fileContent.Suffix, "ResourceManager", LogSeverity::ERROR_MSG );
				m_Allocator->Unwind( marker );
				return nullptr;
			}
		} else {
			m_ResourceLoaderMutex.unlock_shared();
			Logger::Log( "Failed to find resource loader for suffix: " + fileContent.Suffix, "ResourceManager", LogSeverity::ERROR_MSG );
			m_Allocator->Unwind( marker );
			return nullptr;
		}
	} else {
		Resource* resource = resourceIterator->second.Resource;
		resourceIterator->second.ReferenceCount++;
		if ( resourceIterator->second.ReferenceCount == 1 ) {
			resource->ReaddDependencies();
		}
		m_Allocator->Unwind( marker );
		m_ResourceMutex.unlock_shared();
		return resource;
	}

	
	//free( fileContent.Content );
}

void ResourceManager::EvictUntilEnoughMemory() {
	for ( auto resourceIt = m_Resources.begin(); resourceIt != m_Resources.end() && m_MemoryUsage > MAX_MEMORY_USAGE; ) {
		if ( resourceIt->second.ReferenceCount == 0 ) {
			m_MemoryUsage -= resourceIt->second.Resource->GetSize();
			delete resourceIt->second.Resource; // TODOJM: Use correct delete
			resourceIt = m_Resources.erase( resourceIt );
			Logger::Log( "Evicting resource " + LookupResourceName( resourceIt->first ), "ResourceManager", LogSeverity::DEBUG_MSG );
		} else {
			resourceIt++;
		}
	}
	if ( m_MemoryUsage > MAX_MEMORY_USAGE ) { // Could not evict enough
		Logger::Log( "Out of memory for resource manager. Dumping list of resources...", "ResourceManager", LogSeverity::ERROR_MSG );
		std::ofstream dumpFile( "resourceDump.txt" );
		if ( dumpFile.is_open() ) {
			dumpFile << "ReferenceCount\tIdentifier" << std::endl;
			for ( const auto& resource : m_Resources ) {
				dumpFile << resource.second.ReferenceCount << "\t" << LookupResourceName( resource.first ) << std::endl; // TODOJM: Reverse lookup
			}
		}
		dumpFile.close();
		assert( false );
	}
}
