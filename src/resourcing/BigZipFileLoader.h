#pragma once
#include <memory/Alloc.h>
#include <map>
#include <functional>
#include <utility/Config.h>
#include "ResourcingLibraryDefine.h"
#include "Package.h"


typedef struct zzip_dir	ZZIP_DIR;

class BigZipFileLoader : public Package {
public:
	~BigZipFileLoader();

	RESOURCING_API bool Initialize( const pString& path ) override;

	RESOURCING_API FileContent GetFileContent( ResourceIdentifier identifier ) override;

private:
	void Recurse( Config& cfg, std::string scopeString, rMap<pString, Config::ConfigEntry*>* scope );

	const char* ASSET_MAP_FILE_PATH = "AssetMap.cfg";

	ZZIP_DIR* m_Dir = nullptr;
	std::map<ResourceIdentifier, pString> m_AssetPaths;
};
