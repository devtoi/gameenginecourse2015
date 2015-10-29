#include "ResourceManager.h"
#include "ResourceLoader.h"
#include "loader/DDSLoader.h"
#include "loader/ModelLoader.h"
#include "Loader/TextureLoader.h"
#include <utility/Logger.h>
#include "ModelBank.h"
//glcontext handling
#include <utility/PlatformDefinitions.h>
#ifdef PLATFORM == PLATFORM_WINDOWS
#include <SDL2/SDL_syswm.h>
#include <GL/wglew.h>
HGLRC LoadingContext;
HGLRC MainContext;
HDC Device;
#elif PLATFORM == PLATFORM_LINUX
#include <GL/glxew.h
typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
GLXContext MainContext;
GLXContext LoadingContext;
#endif

ResourceManager& ResourceManager::GetInstance() {
	static ResourceManager resourceManager;
	return resourceManager;
}

ResourceManager::ResourceManager() {
	// LSR new for all resource loaders
	AddResourceLoader( std::make_unique<DDSLoader>(), { "dds" } );
	AddResourceLoader( std::make_unique<ModelLoader>(), { "obj" } );
	AddResourceLoader(std::make_unique<TextureLoader>(), { "png" });
}

ResourceManager::~ResourceManager() {
	m_ResourceLoaders.clear();
	m_ResourceLoaderMapping.clear();
	UnloadAllResources();
}

void ResourceManager::WorkerThread() {
	wglMakeCurrent(Device, LoadingContext);
	while (true) {
		//lock mutex for queue
		m_JobQueueMutex.lock();
		if (m_JobQueue.size() > 0) {
			ResourceJob job = m_JobQueue.front();
			m_JobQueue.pop();
			m_JobQueueMutex.unlock();
			auto loaderIterator = m_ResourceLoaderMapping.find(job.File.Suffix);
			if (loaderIterator != m_ResourceLoaderMapping.end()) {
				Logger::Log("Loaded resource", "ResourceManager", LogSeverity::DEBUG_MSG); // TODOJM: Reverse lookup name
				job.Entry->ReferenceCount = 0;
				job.Entry->Resource = loaderIterator->second->LoadResource(job.File);
				auto it = m_Resources.emplace(job.File.ID, std::move(*job.Entry));
			}
			else {
				Logger::Log("Failed to find resource loader for suffix: " + job.File.Suffix, "ResourceManager", LogSeverity::ERROR_MSG);
				return;
			}
		} else {
			m_JobQueueMutex.unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void ResourceManager::StartWorkerThread(SDL_Window* window) {
#ifdef PLATFORM == PLATFORM_WINDOWS
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(window, &info) < 0) {
		assert(false);
	}
	HWND hWnd = info.info.win.window;
	Device = GetDC(hWnd);
	MainContext = wglGetCurrentContext();
	LoadingContext = wglCreateContext(Device);
	wglShareLists(MainContext, LoadingContext);
#elif PLATFORM == PLATFORM_LINUX
	//TODO:Fill out with GLX code
	return;
#endif
	m_WorkerThread = std::thread(&ResourceManager::WorkerThread, this);
	m_WorkerThread.detach();
}

void ResourceManager::PostQuitJob() {
	ResourceJob job;
	FileContent fc;
	fc.ID = WORKER_THREAD_STOP;
	job.File = fc;
	m_JobQueueMutex.lock();
	m_JobQueue.push(job);
	m_JobQueueMutex.unlock();
}

void ResourceManager::UnloadAllResources() {
	for ( auto& resource : m_Resources ) {
		Logger::Log( "Resource: " + rToString( resource.first ) + " is still loaded, please release it proberly", "ResourceManager", LogSeverity::WARNING_MSG );
        if ( resource.second.Resource ) {
            //ReleaseResource( resource.first );
        }
	}
	m_Resources.clear();
}

void ResourceManager::AquireResource( const ResourceIdentifier identifier ) {
	//std::lock( m_ResourceLoaderMutex, m_ResourcesMutex );
	//std::lock_guard<std::mutex> resourceLock( m_ResourcesMutex, std::adopt_lock );
	
	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator == m_Resources.end() ) {
		m_PackageMutex.lock();
		FileContent fileContent = m_PackageManager.GetFileContent( identifier );
		m_PackageMutex.unlock();
		if ( fileContent.Loaded ) {
			//create job
			ResourceJob job;
			job.Entry = new ResourceEntry();
			job.Entry->Resource = new Resource();
			job.File = fileContent;

			m_JobQueueMutex.lock();
			m_JobQueue.push(job);
			m_JobQueueMutex.unlock();

			return;
		} else {
			return;
		}
	} else {
		resourceIterator->second.ReferenceCount++;
		return;
	}
}

void ResourceManager::ReleaseResource( const ResourceIdentifier identifier ) {
	std::lock_guard<std::mutex> resourceLock( m_ResourceMutex );
	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator != m_Resources.end() ) {
		resourceIterator->second.ReferenceCount--;
		if ( resourceIterator->second.ReferenceCount <= 0 ) {
			m_Resources.erase( resourceIterator );
			Logger::Log( "Unloaded resource", "ResourceManager", LogSeverity::DEBUG_MSG ); // TODOJM: Reverse lookup name
		}
	} else {
		Logger::Log( "Tried to release a resource that was not loaded", "ResourceManager", LogSeverity::WARNING_MSG );
	}
}

Resource* ResourceManager::GetResourcePointer( const ResourceIdentifier identifier ) {
	std::lock_guard<std::mutex> resourceLock(m_ResourceMutex);
	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator != m_Resources.end() ) {
		return resourceIterator->second.Resource;
	} else {
		return nullptr;
	}
}

void ResourceManager::AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader, std::initializer_list<pString> fileSuffixes ) {
	//std::lock_guard<std::mutex> resourceLoaderLock( m_ResourceLoaderMutex );
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
	//std::lock_guard<std::mutex> resourceLock( m_ResourcesMutex );
	size_t size = 0;
	for ( const auto& resource : m_Resources ) {
		size += resource.second.Resource->GetSize();
	}
	return size;
}
