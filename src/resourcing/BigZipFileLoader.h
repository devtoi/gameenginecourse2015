#pragma once
#include <memory/Alloc.h>
#include <map>
#include <functional>
#include "ResourcingLibraryDefine.h"

#define Hash( aString ) std::hash<std::string>()( aString )

typedef struct zzip_dir	ZZIP_DIR;

// The reasoning behind haveing a separate id for this is that there probably -
// shouldn't be a direct connection between the resource on file and the resource in memory.
typedef uint64_t AssetIndentifier;

class BigZipFileLoader {
public:

	//Should be called by the resource loader, or something
	RESOURCING_API static BigZipFileLoader& GetInstance();

	BigZipFileLoader();
	~BigZipFileLoader();

	RESOURCING_API bool LoadMetaData( const char* path );

	RESOURCING_API void* GetFileContent( AssetIndentifier key, uint32_t& outSize );
	RESOURCING_API void* GetFileContent( const char* key, uint32_t& outSize );

private:
	ZZIP_DIR* m_Dir;
	std::map<AssetIndentifier, std::string> m_AssetPaths;
};