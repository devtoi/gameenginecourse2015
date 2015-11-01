#include "PackageManager.h"
#include "Package.h"
#include "BigZipFileLoader.h"
#include "CustomBigFileLoader.h"
#include <utility/Logger.h>

PackageManager::PackageManager() {
	m_Packages.push_back( std::make_unique<BigZipFileLoader>() );
	m_Packages.back().get()->Initialize( "../../../asset/Assets.zip" );
	m_Packages.push_back( std::make_unique<CustomBigFileLoader>() );
	m_Packages.back().get()->Initialize( "../../../asset/assets.big" );
}

PackageManager::~PackageManager() {
	m_Packages.clear();
}

FileContent PackageManager::GetFileContent( const ResourceIdentifier identifier ) {
	FileContent fileContent = INVALID_FILE_CONTENT;
	for ( const auto& package : m_Packages ) {
		fileContent = package->GetFileContent( identifier );
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
