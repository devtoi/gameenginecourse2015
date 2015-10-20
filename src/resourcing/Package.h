#pragma once
#include <memory/Alloc.h>
#include "ResourcingLibraryDefine.h"
#include "ResourceTypes.h"
#include "FileContent.h"

class Package {
public:
	virtual bool Initialize( const pString& path ) = 0;

	virtual FileContent GetFileContent( ResourceIdentifier key ) = 0;

private:
};
