#include "ModelLoader.h"
#include <gfx/ModelBank.h>
#include "../Resource.h"

ModelLoader::ModelLoader(){
}

ModelLoader::~ModelLoader(){
}

RESOURCING_API std::unique_ptr<Resource> ModelLoader::LoadResource(const FileContent& fileContent){
	gfx::ModelHandle* h = new gfx::ModelHandle();
	*h = gfx::g_ModelBank.LoadModel(fileContent.Content, fileContent.Size, "a test name");
	return std::unique_ptr<Resource>( (Resource*)h );
}
