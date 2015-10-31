#include "ModelLoader.h"
#include "../Resource.h"
#include "../ModelBank.h"
#include "../ResourceManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

ModelLoader::ModelLoader(){

}

ModelLoader::~ModelLoader(){
}

RESOURCING_API Resource* ModelLoader::LoadResource(const FileContent& fileContent){
	Assimp::Importer loader;
	ModelResource* model = new ModelResource();
	const aiScene* scene = loader.ReadFileFromMemory(fileContent.Content, fileContent.Size, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);
	if (scene && scene->HasMeshes()) {
		LoadMeshes(*model, scene);
		LoadMaterials(*model, scene);
	} else if (!scene) {
		printf("Error loading model \n");
		return nullptr;
	}
	g_ModelBank.AddModel(model, fileContent.ID);
	model->SetReady();
	model->CalcSize();
	return model;
}

void ModelLoader::LoadDefaultMaterial()
{

}


void ModelLoader::LoadMeshes(ModelResource& model, const aiScene* scene) {
	Mesh modelMesh;
	int size = 0;
	int indices = 0;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];
		unsigned int numVertices = 0;
		unsigned int numIndices = 0;
		//foreach vertice
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
			Vertex vertex;
			vertex.Position.x = mesh->mVertices[v].x;
			vertex.Position.y = mesh->mVertices[v].y;
			vertex.Position.z = mesh->mVertices[v].z;
			if (mesh->HasNormals()) {
				vertex.Normal.x = mesh->mNormals[v].x;
				vertex.Normal.y = mesh->mNormals[v].y;
				vertex.Normal.z = mesh->mNormals[v].z;
			}
			if (mesh->HasTangentsAndBitangents()) {
				vertex.Tangent.x = mesh->mTangents[v].x;
				vertex.Tangent.y = mesh->mTangents[v].y;
				vertex.Tangent.z = mesh->mTangents[v].z;
			}
			if (mesh->mTextureCoords[0] != NULL) {
				vertex.TexCoord.x = mesh->mTextureCoords[0][v].x;
				vertex.TexCoord.y = mesh->mTextureCoords[0][v].y;
			}
			numVertices++;
			model.Vertices.push_back(vertex);
		}//end foreach vertice
		 //Indices
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			//index = (Num vertices from the already loaded models) + (Size of all the already loaded meshes + mesh->faceindices)
			model.Indices.push_back( size + mesh->mFaces[f].mIndices[0]);
			model.Indices.push_back( size + mesh->mFaces[f].mIndices[1]);
			model.Indices.push_back( size + mesh->mFaces[f].mIndices[2]);
			numIndices += 3;
		}
		modelMesh.Material = mesh->mMaterialIndex;
		modelMesh.IndexOffset = indices;
		size += numVertices;
		indices += numIndices;
		modelMesh.VertexCount = numVertices;
		modelMesh.IndexCount = numIndices;
		model.Meshes.push_back(modelMesh);
	}//end foreach mesh
}

void ModelLoader::LoadMaterials(ModelResource& model, const aiScene* scene) {
	auto attachResource = [&] ( const char* resourceName ) {
		ResourceIdentifier identifier = HashResourceName( resourceName );
		g_ResourceManager.AquireResource(identifier);
		model.AddDependency( identifier );
	};
	if (!m_HasDefault) {
		attachResource("Texture.WhitePixel");
		attachResource("Texture.Normal");
		attachResource("Texture.Roughness");
		attachResource("Texture.Metal");
		m_HasDefault = true;
	}
	if (scene->mNumMaterials == 0) {
		Material* defaultMat = new Material();
		defaultMat->SetAlbedoTexture(HashResourceName("Texture.WhitePixel"));
		defaultMat->SetNormalTexture(HashResourceName("Texture.Normal"));
		defaultMat->SetRoughnessTexture(HashResourceName("Texture.Roughness"));
		defaultMat->SetMetalTexture(HashResourceName("Texture.Metal"));
		model.Materials.push_back(defaultMat);
	}
	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		Material* modelMat = new Material();
		const aiMaterial* mat = scene->mMaterials[i];
		//get name
		aiString name;
		mat->Get(AI_MATKEY_NAME, name);
		modelMat->SetName(std::string(name.C_Str()));
		//albedo
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullpath = path.data;
				attachResource(path.data);
				modelMat->SetAlbedoTexture(HashResourceName(path.data));
			}
		}
		else {
			modelMat->SetAlbedoTexture(HashResourceName("Texture.WhitePixel"));
		}
		//normal map
		if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
			aiString path;
			if (mat->GetTexture(aiTextureType_HEIGHT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullpath = path.data;
				attachResource(path.data);
				modelMat->SetNormalTexture(HashResourceName(path.data));
			}
		} else {
			modelMat->SetNormalTexture(HashResourceName("Texture.Normal"));
		}
		//roughness map
		if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			aiString path;
			if (mat->GetTexture(aiTextureType_SPECULAR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullpath = path.data;
				attachResource(path.data);
				modelMat->SetRoughnessTexture(HashResourceName(path.data));
			}
		} else {
			modelMat->SetRoughnessTexture(HashResourceName("Texture.Roughness"));
		}
		//Metal map
		if (mat->GetTextureCount(aiTextureType_AMBIENT) > 0) { //use ambient texture as metal map for now
			aiString path;
			if (mat->GetTexture(aiTextureType_AMBIENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullpath = path.data;
				attachResource(path.data);
				modelMat->SetMetalTexture(HashResourceName(path.data));
			}
		}else {
			modelMat->SetMetalTexture(HashResourceName("Texture.Metal"));
		}
		model.Materials.push_back(modelMat);
	}
}
