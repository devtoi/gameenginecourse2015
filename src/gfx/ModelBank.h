#pragma once
#include "Model.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GFXLibraryDefine.h"

/*
This class loads and store all models and vertices and indices.
It also generates vertex and index buffers.
*/
namespace gfx {
typedef int ModelHandle;
#define g_ModelBank ModelBank::GetInstance()
class ModelBank {
  public:
	GFX_API ~ModelBank();
	GFX_API static ModelBank& GetInstance();
	GFX_API const Model& FetchModel(ModelHandle handle);
	GFX_API ModelHandle LoadModel(const char* filename);
	GFX_API ModelHandle AddModel(Model& TheModel);
	GFX_API void UpdateModel(ModelHandle& handle, Model& model);
	GFX_API void BuildBuffers();
	GFX_API void DeleteModel();
	GFX_API void ApplyBuffers();
	GFX_API void Clear();
	GFX_API std::vector<VertexPosNormalTexTangent>& GetVertices();
	GFX_API  VertexBuffer& GetVertexBuffer();
	GFX_API GLuint& GetIndexBuffer();
  private:
	ModelBank();
	void LoadMeshes(Model& model, const aiScene* scene);
	VertexBuffer							m_VertexBuffer;
	GLuint									m_IndexBuffer;
	ModelHandle								m_Numerator;
	std::map<ModelHandle, Model>			m_Models;
	std::vector<VertexPosNormalTexTangent>	m_Vertices;
	std::vector<GLuint>						m_Indices;
	int										m_VerticesWhenLastBuilt;
};
}
