#include "ResourceManager.h"
#include "ResourceLoader.h"
#include "loader/DDSLoader.h"
#include "loader/ModelLoader.h"
#include "loader/TextureLoader.h"
#include <utility/Logger.h>
#include "ModelBank.h"
//glcontext handling
#include <utility/PlatformDefinitions.h>

#if PLATFORM == PLATFORM_WINDOWS
    #include "SDL_syswm.h"
    #include <GL/wglew.h>
    HGLRC LoadingContext;
    HGLRC MainContext;
    HDC Device;
#elif PLATFORM == PLATFORM_LINUX
    #include <GL/glxew.h>
    #include "SDL_syswm.h"
    typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
    //GLXContext g_MainContext;
    //GLXContext g_LoadingContext;
    //Display* g_Display;
    //Window g_Window;
	SDL_GLContext g_SDLLoadContext;
	//SDL_Window* g_MainWindow;
#endif

ResourceManager& ResourceManager::GetInstance() {
	static ResourceManager resourceManager;
	return resourceManager;
}

ResourceManager::ResourceManager() {
	// LSR new for all resource loaders
	AddResourceLoader( std::make_unique<DDSLoader>(), { "dds" } );
	AddResourceLoader( std::make_unique<ModelLoader>(), { "obj", "dae" } );
	AddResourceLoader(std::make_unique<TextureLoader>(), { "png" });
}

ResourceManager::~ResourceManager() {
	m_ResourceLoaders.clear();
	m_ResourceLoaderMapping.clear();
	UnloadAllResources();
}

void ResourceManager::WorkerThread( SDL_Window* window ) {
#if PLATFORM == PLATFORM_LINUX
	//g_Display = info.info.x11.display;
	//g_Window = info.info.x11.window;
	//g_MainContext = glXGetCurrentContext();

	//// Get a matching FB config
	//static int visual_attribs[] =
	//{
		//GLX_X_RENDERABLE    , True,
		//GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		//GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		//GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		//GLX_RED_SIZE        , 8,
		//GLX_GREEN_SIZE      , 8,
		//GLX_BLUE_SIZE       , 8,
		//GLX_ALPHA_SIZE      , 8,
		//GLX_DEPTH_SIZE      , 24,
		//GLX_STENCIL_SIZE    , 8,
		//GLX_DOUBLEBUFFER    , True,
		////GLX_SAMPLE_BUFFERS  , 1,
		////GLX_SAMPLES         , 4,
		//None
	//};

	//int fbcount;
	//GLXFBConfig* fbc = glXChooseFBConfig(g_Display, DefaultScreen(g_Display), visual_attribs, &fbcount);
	//if (!fbc)
	//{
		//printf( "Failed to retrieve a framebuffer config\n" );
		//exit(1);
	//}
	//printf( "Found %d matching FB configs.\n", fbcount );

	//// Pick the FB config/visual with the most samples per pixel
	//printf( "Getting XVisualInfos\n" );
	//int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	//int i;
	//for (i=0; i<fbcount; ++i)
	//{
		//XVisualInfo *vi = glXGetVisualFromFBConfig( g_Display, fbc[i] );
		//if ( vi )
		//{
			//int samp_buf, samples;
			//glXGetFBConfigAttrib( g_Display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
			//glXGetFBConfigAttrib( g_Display, fbc[i], GLX_SAMPLES       , &samples  );

			//printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
					//" SAMPLES = %d\n", 
					//i, vi -> visualid, samp_buf, samples );

			//if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
				//best_fbc = i, best_num_samp = samples;
			//if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
				//worst_fbc = i, worst_num_samp = samples;
		//}
		//XFree( vi );
	//}

	//GLXFBConfig bestFbc = fbc[ best_fbc ];

	//// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
	//XFree( fbc );

	//// Get a visual
	//XVisualInfo *vi = glXGetVisualFromFBConfig( g_Display, bestFbc );

	//g_LoadingContext = glXCreateContext( g_Display, vi, g_MainContext, True );
	//XFree( vi );

#endif
#if PLATFORM == PLATFORM_WINDOWS
	wglMakeCurrent(Device, LoadingContext);
#elif PLATFORM == PLATFORM_LINUX
	//glXMakeCurrent( g_Display, g_Window, g_LoadingContext );
	SDL_GL_MakeCurrent( window, g_SDLLoadContext );
#endif
	while (true) {
		//lock mutex for queue
		m_JobQueueMutex.lock();
		if (m_JobQueue.size() > 0) {
			ResourceJob job = m_JobQueue.front();
			m_JobQueue.pop();
			m_JobQueueMutex.unlock();
			if (job.File.ID == WORKER_THREAD_STOP)
				return;
			auto loaderIterator = m_ResourceLoaderMapping.find(job.File.Suffix);
			if (loaderIterator != m_ResourceLoaderMapping.end()) {
				Logger::Log("Loaded resource with suffix: " + job.File.Suffix, "ResourceManager", LogSeverity::DEBUG_MSG); // TODOJM: Reverse lookup name
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
	}
}

void ResourceManager::StartWorkerThread( SDL_Window* window, SDL_GLContext mainContext ) {
#if PLATFORM == PLATFORM_WINDOWS
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
#endif
#if PLATFORM == PLATFORM_LINUX
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(window, &info) < 0) {
		assert(false);
	}
	SDL_GL_MakeCurrent( window, mainContext );
	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
	g_SDLLoadContext = SDL_GL_CreateContext( window );
	SDL_GL_MakeCurrent( window, mainContext );
#endif

	m_WorkerThread = std::thread(&ResourceManager::WorkerThread, this, window);
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
		Logger::Log( "Resource: " + rToString( resource.first ) + " is still loaded, please release it properly", "ResourceManager", LogSeverity::WARNING_MSG );
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
