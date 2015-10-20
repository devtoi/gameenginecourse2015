#pragma once 
#include "../ResourceLoader.h"
#include "../ResourcingLibraryDefine.h"
class ModelLoader : public ResourceLoader {
public:
	RESOURCING_API ModelLoader();
	RESOURCING_API ~ModelLoader();
	RESOURCING_API virtual Resource* LoadResource(const FileContent& fileContent);
};