#include "ResourceManager.h"
#include "Resource.h"
#include "ResourceLoader.h"

ResourceManager& ResourceManager::GetInstance() {
	static ResourceManager resourceManager;
	return resourceManager;
}

ResourceManager::ResourceManager() {
	// LSR new for all resource loaders
}

ResourceManager::~ResourceManager() {
	m_ResourceLoaders.clear();
	for ( auto& resource : m_Resources ) {
		ReleaseResource( resource.first );
	}
	m_Resources.clear();
	m_ResourceLoaderMapping.clear();
}

Resource* ResourceManager::AquireResource( const ResourceIdentifier identifier ) {
	return nullptr;
}

void ResourceManager::ReleaseResource( const ResourceIdentifier identifier ) {
}

Resource* ResourceManager::GetResourcePointer( const ResourceIdentifier identifier ) {
	return nullptr;
}

void ResourceManager::AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader ) {
}
