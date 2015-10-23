#pragma once 
#include "../ResourceLoader.h"
#include "../ResourcingLibraryDefine.h"
class ModelLoader : public ResourceLoader {
public:
	RESOURCING_API ModelLoader();
	RESOURCING_API ~ModelLoader();
	RESOURCING_API std::unique_ptr<Resource> LoadResource( const FileContent& fileContent ) override;
};
