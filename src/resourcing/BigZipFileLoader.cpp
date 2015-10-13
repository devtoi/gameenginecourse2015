#include "BigZipFileLoader.h"
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <zzip/zzip.h>
#include <utility/Config.h>

BigZipFileLoader& BigZipFileLoader::GetInstance() {
	static BigZipFileLoader instance;
	return instance;
}

BigZipFileLoader::BigZipFileLoader() {
}

BigZipFileLoader::~BigZipFileLoader() {
	if( m_Dir ) { 
		zzip_dir_close( m_Dir );
	}
}

void Recurse( Config& cfg, std::string scopeString, rMap<std::string, Config::ConfigEntry*>* scope, std::map<AssetIndentifier, std::string>& assetMap ){
	for( auto& entry : *scope ) {
		std::string tscopeString = scopeString + "." + entry.first;

		if( entry.second->Type == Config::Type::Map ) {
			tscopeString = scopeString == "" ? entry.first : scopeString + "." + entry.first;

			auto map = cfg.GetScopeMap( "Assets." + tscopeString );

			Recurse( cfg, tscopeString, map, assetMap );
		} else {
			assetMap[Hash( tscopeString )] = entry.second->Value.StringVal;

			AssetIndentifier hashed = Hash( tscopeString ); 
			int i = 0;
		}
	}

}

bool BigZipFileLoader::LoadMetaData( const char * path ) {
	
	m_Dir = zzip_dir_open( path, 0 );
	if ( !m_Dir ) {
		return false;
	}

	ZZIP_FILE* fp = zzip_file_open( m_Dir, "AssetMap.cfg", 0 );
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
	bool result = cfg.ReadFileFromMemory( buffer, "AssetMap.cfg" );

	auto assetScope = cfg.GetScopeMap( "Assets" );

	rString scopeString = "";
	Recurse( cfg, scopeString, assetScope, m_AssetPaths );

	return result;
}

void* BigZipFileLoader::GetFileContent( AssetIndentifier key, uint32_t& outSize ) {
	if( !m_Dir ) {
		printf( "ERROR: Couldn't open bigfile" );
		return nullptr;
	}

	std::string path;
	auto it = m_AssetPaths.find( key );
	if( it != m_AssetPaths.end() ) {
		path = m_AssetPaths[key];
	}

	if( path != "" ) {
		ZZIP_FILE* fp = zzip_file_open( m_Dir, path.c_str(), 0 );
		if ( !fp ) {
			return nullptr;
		}

		ZZIP_STAT stats;
		zzip_file_stat( fp, &stats );

		int length = stats.st_size;
		void* buffer = malloc( length );

		zzip_ssize_t len = zzip_file_read( fp, buffer, length );

		zzip_file_close( fp );

		return buffer;
	}

	return nullptr;
}

//Example key: Texture.Sky.Sky001
void * BigZipFileLoader::GetFileContent( const char* key, uint32_t& outSize ) {
	return GetFileContent( Hash( std::string( key ) ), outSize );

	/*std::string val;

	rIStringStream iss( key );
	getline( iss, val, '.' );
*/
	
}
