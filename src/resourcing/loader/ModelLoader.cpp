#include "ModelLoader.h"
#include "../Resource.h"

ModelLoader::ModelLoader(){
}

ModelLoader::~ModelLoader(){
}

RESOURCING_API std::unique_ptr<Resource> ModelLoader::LoadResource(const FileContent& fileContent){
	return std::unique_ptr<Resource>(nullptr);
}
