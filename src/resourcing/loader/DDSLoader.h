#pragma once
#include "../ResourceLoader.h"
#include "../ResourcingLibraryDefine.h"
#include <memory>
class DDSLoader : public ResourceLoader {
public:
	RESOURCING_API DDSLoader();
	RESOURCING_API ~DDSLoader();
	RESOURCING_API Resource* LoadResource( const FileContent& fileContent ) override;
	//RESOURCING_API void LoadCompleteDDS(const char* filename);
private:
};
