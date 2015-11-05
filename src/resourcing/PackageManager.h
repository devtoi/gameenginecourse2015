#pragma once
#include <memory>

#include "ResourceTypes.h"
#include "FileContent.h"

class DrinQStackAllocator;
class Package;

class PackageManager {
public:
	PackageManager( DrinQStackAllocator* allocator );
	~PackageManager();

	FileContent GetFileContent( const ResourceIdentifier identifier, DrinQStackAllocator* allocator );
private:
	std::vector<std::unique_ptr<Package>> m_Packages;
};
