#pragma once
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <thread>
#include <memory/Alloc.h>
#include "SDL.h"
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

	RESOURCING_API void AquireResource( const ResourceIdentifier identifier );
	RESOURCING_API void ReleaseResource( const ResourceIdentifier identifier );
	RESOURCING_API Resource* GetResourcePointer( const ResourceIdentifier identifier );

	RESOURCING_API void AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader, std::initializer_list<pString> fileSuffixes );

	RESOURCING_API size_t GetTotalResourceSize( ) const;

	RESOURCING_API void StartWorkerThread( SDL_Window* window, SDL_GLContext mainContext );
	RESOURCING_API void PostQuitJob();

private:
	ResourceManager();
	~ResourceManager();
	void WorkerThread( SDL_Window* window );

	struct ResourceEntry {
		int ReferenceCount = 0;
		Resource* Resource;
	};

	struct ResourceJob {
		FileContent File;
		ResourceEntry* Entry;
	};

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

	const size_t WORKER_THREAD_STOP = 10;
};
