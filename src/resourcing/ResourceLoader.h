#pragma once
#include <memory/Alloc.h>
#include "ResourceTypes.h"
#include "FileContent.h"

class Resource;

class ResourceLoader {
public:
	virtual Resource* LoadResource( const FileContent& fileContent ) = 0;
};
