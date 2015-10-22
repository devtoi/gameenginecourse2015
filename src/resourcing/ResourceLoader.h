#pragma once
#include <memory>
#include <memory/Alloc.h>
#include "ResourceTypes.h"
#include "FileContent.h"

class Resource;

class ResourceLoader {
public:
	virtual std::unique_ptr<Resource> LoadResource( const FileContent& fileContent ) = 0;
};
