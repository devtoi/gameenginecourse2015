#include "PackageManager.h"
#include "Package.h"
#include "BigZipFileLoader.h"
#include <utility/Logger.h>

PackageManager::PackageManager() {
	m_Packages.push_back( std::make_unique<BigZipFileLoader>() );
}

PackageManager::~PackageManager() {
	m_Packages.clear();
}

FileContent PackageManager::GetFileContent( const ResourceIdentifier identifier ) {
	FileContent fileContent = INVALID_FILE_CONTENT;
	for ( const auto& package : m_Packages ) {
		fileContent = package->GetFileContent( identifier );
		if ( fileContent.Loaded ) {
			break;
		}
	}
	if ( fileContent.Loaded == false ) {
		Logger::Log( "Failed to load file content", "PackageManager", LogSeverity::ERROR_MSG ); // TODOJM: Print actual file name
	}
	return fileContent;
}
