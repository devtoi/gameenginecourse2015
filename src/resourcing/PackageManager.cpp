#include "PackageManager.h"
#include "Package.h"
#include "BigZipFileLoader.h"
#include "CustomBigFileLoader.h"
#include <utility/Logger.h>

PackageManager::PackageManager( DrinQStackAllocator* allocator ) {

	m_Packages.push_back( std::make_unique<BigZipFileLoader>() );
	m_Packages.back().get()->Initialize( "../../../asset/Assets.zip", allocator );
	m_Packages.push_back( std::make_unique<CustomBigFileLoader>() );
	m_Packages.back().get()->Initialize( "../../../asset/assets.big", allocator );
}

PackageManager::~PackageManager() {
	m_Packages.clear();
}

FileContent PackageManager::GetFileContent( const ResourceIdentifier identifier, DrinQStackAllocator* allocator ) {
	FileContent fileContent = INVALID_FILE_CONTENT;
	for ( const auto& package : m_Packages ) {
		fileContent = package->GetFileContent( identifier, allocator );
		fileContent.ID = identifier;
		if ( fileContent.Loaded ) {
			break;
		}
	}
	if ( fileContent.Loaded == false ) {
		Logger::Log( "Failed to load file content", "PackageManager", LogSeverity::ERROR_MSG ); // TODOJM: Print actual file name
	}
	return fileContent;
}
