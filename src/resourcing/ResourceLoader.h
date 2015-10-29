#pragma once
#include <memory>
#include <memory/Alloc.h>
#include "ResourceTypes.h"
#include "ResourceManager.h"

class Resource;

class ResourceLoader {
public:
	virtual Resource* LoadResource( const FileContent& fileContent) = 0;
};
