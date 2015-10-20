#include "ModelLoader.h"
#include "gfx/ModelBank.h"
ModelLoader::ModelLoader(){
}

ModelLoader::~ModelLoader(){
}

RESOURCING_API Resource * ModelLoader::LoadResource(const FileContent& fileContent){
	gfx::ModelHandle* h = new gfx::ModelHandle();
	*h = gfx::g_ModelBank.LoadModel(fileContent.Content, fileContent.Size, "a test name");
	return (Resource*)h;
}
