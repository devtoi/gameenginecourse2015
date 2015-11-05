#pragma once
#include <memory/Alloc.h>
#include "ResourcingLibraryDefine.h"
#include "ResourceTypes.h"
#include "FileContent.h"

class DrinQStackAllocator;

class Package {
public:
	virtual bool Initialize( const pString& path, DrinQStackAllocator* allocator ) = 0;

	virtual FileContent GetFileContent( ResourceIdentifier key, DrinQStackAllocator* allocator ) = 0;

private:
};
