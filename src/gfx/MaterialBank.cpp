#include "MaterialBank.h"
#include <fstream>
#include <assimp/scene.h>
#include "Model.h"
#include "Material.h"
#include "gfxutility.h"
#include <resourcing/ResourceManager.h>
gfx::MaterialBank::MaterialBank() {
}

gfx::MaterialBank::~MaterialBank() {
	ClearMaterials();
}

gfx::MaterialBank& gfx::MaterialBank::GetInstance() {
	static MaterialBank m_Bank;
	return m_Bank;
}
void gfx::MaterialBank::Initialize(){
	m_DefaultAlbedo		= (TextureResource*)g_ResourceManager.AquireResource(HashResourceName("Texture.WhitePixel"));
	m_DefaultNormal		= (TextureResource*)g_ResourceManager.AquireResource(HashResourceName("Texture.Normal"));
	m_DefaultRoughness	= (TextureResource*)g_ResourceManager.AquireResource(HashResourceName("Texture.Roughness"));
	m_DefaultMetal		= (TextureResource*)g_ResourceManager.AquireResource(HashResourceName("Texture.Metal"));


}
void gfx::MaterialBank::LoadMaterials(Model& model, std::string filename, const aiScene* scene) {
	model.MaterialOffset = (unsigned int)m_Materials.size();
	if (scene->mNumMaterials == 0) {
		Material* defaultMat = new Material();
		defaultMat->SetAlbedoTexture(m_DefaultAlbedo);
		defaultMat->SetNormalTexture(m_DefaultNormal);
		defaultMat->SetRoughnessTexture(m_DefaultRoughness);
		defaultMat->SetMetalTexture(m_DefaultMetal);
		m_Materials.push_back(defaultMat);
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
				modelMat->SetAlbedoTexture((TextureResource*)g_ResourceManager.AquireResource(HashResourceName(path.data)));
			}
		} else {
			modelMat->SetAlbedoTexture(m_DefaultAlbedo);
		}
		//normal map
		if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
			aiString path;
			if (mat->GetTexture(aiTextureType_HEIGHT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullpath = path.data;
				modelMat->SetNormalTexture((TextureResource*)g_ResourceManager.AquireResource(HashResourceName(path.data)));
			}
		} else {
			modelMat->SetNormalTexture(m_DefaultNormal);
		}
		//roughness map
		if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			aiString path;
			if (mat->GetTexture(aiTextureType_SPECULAR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullpath = path.data;
				modelMat->SetRoughnessTexture((TextureResource*)g_ResourceManager.AquireResource(HashResourceName(path.data)));
			}
		} else {
			modelMat->SetRoughnessTexture(m_DefaultRoughness);
		}
		//Metal map
		if (mat->GetTextureCount(aiTextureType_AMBIENT) > 0) { //use ambient texture as metal map for now
			aiString path;
			if (mat->GetTexture(aiTextureType_AMBIENT, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullpath = path.data;
				modelMat->SetMetalTexture((TextureResource*)g_ResourceManager.AquireResource(HashResourceName(path.data)));
			}
		} else {
			modelMat->SetMetalTexture(m_DefaultMetal);
		}
		m_Materials.push_back(modelMat);
	}
}

void gfx::MaterialBank::ClearMaterials() {
	for (int i = 0; i < m_Materials.size(); i++) {
		delete m_Materials[i];
	}
	for (int i = 0; i < m_Textures.size(); i++) {
		delete m_Textures[i];
	}
	m_Materials.clear();
	m_MatMap.clear();
}


gfx::Material* gfx::MaterialBank::GetMaterial(int matIndex) {
	if(matIndex == -1)
		return nullptr;
	return m_Materials[matIndex];
}

gfx::Material* gfx::MaterialBank::GetMaterial(const std::string& name) {

	std::map<std::string, Material*>::iterator it = m_MatMap.find(name);
	if(it != m_MatMap.end()) {
		return m_MatMap[name];
	} else
		return nullptr;
}

TextureHandle gfx::MaterialBank::LoadTexture(const char* filename, TextureType type) {
	for (int i = 0; i < m_Textures.size(); ++i){
		if (m_Textures[i]->GetFilename() == std::string(filename)) {
			return i;
		}
	}
	Texture* tex = new Texture();
	tex->Init(filename, type);
	m_Textures.push_back(tex);
	return m_Numerator++;
}

gfx::Texture* gfx::MaterialBank::GetTexture(TextureHandle handle) {
	return m_Textures[handle];
}