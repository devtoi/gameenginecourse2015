#pragma once
#include <memory/Alloc.h>
#include <memory>
#include "ResourceTypes.h"

#define g_ResourceManager ResourceManager::GetInstance()

class Resource;
class ResourceLoader;

class ResourceManager {
public:
	static ResourceManager& GetInstance();

	Resource* AquireResource( const ResourceIdentifier identifier );
	void ReleaseResource( const ResourceIdentifier identifier );
	Resource* GetResourcePointer( const ResourceIdentifier identifier );

	void AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader );

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
};
