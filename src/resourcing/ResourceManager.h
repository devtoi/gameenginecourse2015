#pragma once
#include <mutex>
#include <atomic>
#include <shared_mutex>
#include <memory>
#include <thread>
#include "SDL.h"
#include <memory/Alloc.h>
#include <utility/ThreadPool.h>
#include "ResourcingLibraryDefine.h"
#include "Resource.h"
#include "ResourceTypes.h"
#include "PackageManager.h"

#define g_ResourceManager ResourceManager::GetInstance()

class Resource;
class ResourceLoader;

class ResourceManager {
public:
	RESOURCING_API static ResourceManager& GetInstance();

	RESOURCING_API void UnloadAllResources();

	RESOURCING_API std::future<Resource*> AquireResource( const ResourceIdentifier identifier );
	RESOURCING_API void ReleaseResource( const ResourceIdentifier identifier );
	RESOURCING_API Resource* GetResourcePointer( const ResourceIdentifier identifier );

	RESOURCING_API void AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader, std::initializer_list<pString> fileSuffixes );

	RESOURCING_API size_t GetTotalResourceSize( ) const;

	RESOURCING_API void StartWorkerThread( SDL_Window* window, SDL_GLContext mainContext );
	RESOURCING_API void PostQuitJob();

private:
	struct ResourceJob;

	ResourceManager();
	~ResourceManager();

	bool MakeContextCurrent( SDL_Window* window );
	Resource* LoadResource( ResourceJob resourceJob );
	void EvictUntilEnoughMemory( );

	struct ResourceEntry {
		int ReferenceCount = 0;
		Resource* Resource;
	};

	struct ResourceJob {
		FileContent File;
		ResourceEntry* Entry;
	};

	const size_t MAX_MEMORY_USAGE = 256 * 1024 * 1024; //256 mebi bytes

	pVector<std::unique_ptr<ResourceLoader>> m_ResourceLoaders;
	pMap<pString, ResourceLoader*> m_ResourceLoaderMapping;
	std::shared_timed_mutex m_ResourceLoaderMutex;

	pUnorderedMap<ResourceIdentifier, ResourceEntry> m_Resources;
	std::queue<ResourceJob>							 m_JobQueue;
	mutable std::mutex	m_JobQueueMutex;
	mutable std::mutex	m_PackageMutex;
	mutable std::shared_timed_mutex	m_ResourceMutex;
	std::thread			m_WorkerThread;

	PackageManager m_PackageManager;

	std::atomic_size_t m_MemoryUsage;

	const size_t WORKER_THREAD_STOP = 10;

	ThreadPool m_ThreadPool;
	ThreadIdentifier m_LoaderThread;
};
