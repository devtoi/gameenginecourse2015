#include "BigZipFileLoader.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <zzip/zzip.h>
#include <utility/Config.h>
#include <utility/Logger.h>

BigZipFileLoader::~BigZipFileLoader() {
	if( m_Dir ) { 
		zzip_dir_close( m_Dir );
	}
}

void BigZipFileLoader::Recurse( Config& cfg, std::string scopeString, rMap<pString, Config::ConfigEntry*>* scope ) {
	for( auto& entry : *scope ) {
		std::string tscopeString = scopeString + "." + entry.first;

		if( entry.second->Type == Config::Type::Map ) {
			tscopeString = scopeString == "" ? entry.first : scopeString + "." + entry.first;

			auto map = cfg.GetScopeMap( "Assets." + tscopeString );

			Recurse( cfg, tscopeString, map );
		} else {
			ResourceIdentifier hashed = HashResourceName( tscopeString ); 
			m_AssetPaths[hashed] = entry.second->Value.StringVal;
		}
	}
}

bool BigZipFileLoader::Initialize( const pString& path ) {
	
	m_Dir = zzip_dir_open( path.c_str(), 0 );
	if ( !m_Dir ) {
		return false;
	}

	ZZIP_FILE* fp = zzip_file_open( m_Dir, ASSET_MAP_FILE_PATH, 0 );
	if ( !fp ) {
		return false;
	}

	ZZIP_STAT stats;
	zzip_file_stat( fp, &stats );

	int length = stats.st_size;
	char* buffer = new char[length];
	buffer[length] = '\0';
	
	zzip_ssize_t len = zzip_file_read( fp, buffer, length );
	zzip_file_close( fp );
	
	Config cfg;
	bool result = cfg.ReadFileFromMemory( buffer, ASSET_MAP_FILE_PATH );

	auto assetScope = cfg.GetScopeMap( "Assets" );

	rString scopeString = "";
	Recurse( cfg, scopeString, assetScope );

	return result;
}

FileContent BigZipFileLoader::GetFileContent( ResourceIdentifier identifier ) {
	if( !m_Dir ) {
		Logger::Log( "Tried to get content from zip archive that is not loaded", "ZipLoader", LogSeverity::ERROR_MSG );
		return INVALID_FILE_CONTENT;
	}

	std::string path = "";
	auto it = m_AssetPaths.find( identifier );
	if( it != m_AssetPaths.end() ) {
		path = m_AssetPaths[identifier];
	}

	if( path != "" ) {
		ZZIP_FILE* fp = zzip_file_open( m_Dir, path.c_str(), 0 );
		if ( !fp ) {
			return INVALID_FILE_CONTENT;
		}

		ZZIP_STAT stats;
		zzip_file_stat( fp, &stats );

		int length = stats.st_size;
		void* buffer = malloc( length );

		zzip_ssize_t len = zzip_file_read( fp, buffer, length );

		zzip_file_close( fp );

		FileContent fileContent;
		fileContent.Loaded = true;
		fileContent.Size = len;
		fileContent.Content = buffer;

		size_t dotPos = path.find_last_of( "." );
		if ( dotPos != std::string::npos ) {
			fileContent.Suffix = path.substr(dotPos+1);
		}
		return fileContent;
	}

	return INVALID_FILE_CONTENT;
}
