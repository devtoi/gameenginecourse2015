#include "ResourceManager.h"
#include "Resource.h"
#include "ResourceLoader.h"
#include "loader/DDSLoader.h"
#include "loader/ModelLoader.h"
#include <utility/Logger.h>

ResourceManager& ResourceManager::GetInstance() {
	static ResourceManager resourceManager;
	return resourceManager;
}

ResourceManager::ResourceManager() {
	// LSR new for all resource loaders
	AddResourceLoader( std::make_unique<DDSLoader>(), { "dds" } );
	AddResourceLoader(std::make_unique<ModelLoader>(), { "obj" });
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
	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator == m_Resources.end() ) {
		FileContent fileContent = m_PackageManager.GetFileContent( identifier );
		if ( fileContent.Loaded ) {
			auto loaderIterator = m_ResourceLoaderMapping.find( fileContent.Suffix );
			if ( loaderIterator != m_ResourceLoaderMapping.end() ) {
				return loaderIterator->second->LoadResource( fileContent );
			} else {
				Logger::Log( "Failed to find resource loader for suffix: " + fileContent.Suffix, "ResourceManager", LogSeverity::ERROR_MSG );
				return nullptr;
			}
		} else {
			return nullptr;
		}
	} else {
		resourceIterator->second.ReferenceCount += 1;
		return resourceIterator->second.Resource.get();
	}
}

void ResourceManager::ReleaseResource( const ResourceIdentifier identifier ) {
}

Resource* ResourceManager::GetResourcePointer( const ResourceIdentifier identifier ) {
	return nullptr;
}

void ResourceManager::AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader, std::initializer_list<pString> fileSuffixes ) {
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

