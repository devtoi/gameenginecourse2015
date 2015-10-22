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
	AddResourceLoader( std::make_unique<ModelLoader>(), { "obj" } );
}

ResourceManager::~ResourceManager() {
	m_ResourceLoaders.clear();
	for ( auto& resource : m_Resources ) {
        if ( resource.second.Resource ) {
            ReleaseResource( resource.first );
        }
	}
	m_Resources.clear();
	m_ResourceLoaderMapping.clear();
}

Resource* ResourceManager::AquireResource( const ResourceIdentifier identifier ) {
	std::lock( m_ResourceLoaderMutex, m_ResourcesMutex );
	std::lock_guard<std::mutex> resourceLock( m_ResourcesMutex, std::adopt_lock );

	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator == m_Resources.end() ) {
		FileContent fileContent = m_PackageManager.GetFileContent( identifier );
		if ( fileContent.Loaded ) {
			std::lock_guard<std::mutex> resourceLoaderLock( m_ResourceLoaderMutex, std::adopt_lock );
			auto loaderIterator = m_ResourceLoaderMapping.find( fileContent.Suffix );
			if ( loaderIterator != m_ResourceLoaderMapping.end() ) {
				Logger::Log( "Loaded resource", "ResourceManager", LogSeverity::DEBUG_MSG ); // TODOJM: Reverse lookup name
				ResourceEntry re;
				re.ReferenceCount = 0;
				re.Resource = loaderIterator->second->LoadResource( fileContent );
				auto it = m_Resources.emplace( identifier, std::move( re ) );
				return it.first->second.Resource.get();
			} else {
				Logger::Log( "Failed to find resource loader for suffix: " + fileContent.Suffix, "ResourceManager", LogSeverity::ERROR_MSG );
				return nullptr;
			}
		} else {
			return nullptr;
		}
	} else {
		resourceIterator->second.ReferenceCount++;
		return resourceIterator->second.Resource.get();
	}
}

void ResourceManager::ReleaseResource( const ResourceIdentifier identifier ) {
	std::lock_guard<std::mutex> resourceLock( m_ResourcesMutex );
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
	std::lock_guard<std::mutex> resourceLock( m_ResourcesMutex );
	auto resourceIterator = m_Resources.find( identifier );
	if ( resourceIterator != m_Resources.end() ) {
		return resourceIterator->second.Resource.get();
	} else {
		return nullptr;
	}
}

void ResourceManager::AddResourceLoader( std::unique_ptr<ResourceLoader> resourceLoader, std::initializer_list<pString> fileSuffixes ) {
	std::lock_guard<std::mutex> resourceLoaderLock( m_ResourceLoaderMutex );
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
	std::lock_guard<std::mutex> resourceLock( m_ResourcesMutex );
	size_t size = 0;
	for ( const auto& resource : m_Resources ) {
		size += resource.second.Resource.get()->GetSize();
	}
	return size;
}
