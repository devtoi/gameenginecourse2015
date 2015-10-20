#pragma once
#include <memory/Alloc.h>
#include <memory>
#include "ResourcingLibraryDefine.h"
#include "ResourceTypes.h"
#include "PackageManager.h"

#define g_ResourceManager ResourceManager::GetInstance()

class Resource;
class ResourceLoader;

class ResourceManager {
public:
	RESOURCING_API static ResourceManager& GetInstance();

	RESOURCING_API Resource* AquireResource( const ResourceIdentifier identifier );
	RESOURCING_API void ReleaseResource( const ResourceIdentifier identifier );
	RESOURCING_API Resource* GetResourcePointer( const ResourceIdentifier identifier );

	RESOURCING_API void AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader, std::initializer_list<pString> fileSuffixes );

private:
	ResourceManager();
	~ResourceManager();

	struct ResourceEntry {
		int ReferenceCount = 0;
		std::unique_ptr<Resource> Resource;
	};

	pVector<std::unique_ptr<ResourceLoader>> m_ResourceLoaders;
	pMap<pString, ResourceLoader*> m_ResourceLoaderMapping;
	pUnorderedMap<ResourceIdentifier, ResourceEntry> m_Resources;

	PackageManager m_PackageManager;
};
