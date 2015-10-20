#pragma once
#include <memory>

#include "ResourceTypes.h"
#include "FileContent.h"

class Package;

class PackageManager {
public:
	PackageManager();
	~PackageManager();

	FileContent GetFileContent( const ResourceIdentifier identifier );
private:
	std::vector<std::unique_ptr<Package>> m_Packages;
};
