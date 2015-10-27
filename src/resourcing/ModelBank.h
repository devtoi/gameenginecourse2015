#pragma once
#include <map>
#include "resource/ModelResource.h"
#include "ResourcingLibraryDefine.h"
#include "ResourceTypes.h"
/*
This class store all models and vertices and indices.
It also generates vertex and index buffers.
*/
#define g_ModelBank ModelBank::GetInstance()
class ModelBank {
  public:
	RESOURCING_API ~ModelBank();
	RESOURCING_API static ModelBank& GetInstance();
	RESOURCING_API void AddModel(ModelResource* model,const ResourceIdentifier id);
	RESOURCING_API void BuildBuffers();
	RESOURCING_API void DeleteModel(const ResourceIdentifier id);
	RESOURCING_API void ApplyBuffers();
	RESOURCING_API void Clear();
  private:
	ModelBank();
	GLuint											m_VertexBuffer;
	GLuint											m_IndexBuffer;
	GLuint											m_VertexArrayObject;
	std::map<ResourceIdentifier, ModelResource*>	m_Models;
};
