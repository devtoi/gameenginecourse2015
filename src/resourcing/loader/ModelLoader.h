#pragma once 
#include "../ResourceLoader.h"
#include "../ResourcingLibraryDefine.h"
#include "../resource/ModelResource.h"
#include <assimp/scene.h>
class ModelLoader : public ResourceLoader {
public:
	RESOURCING_API ModelLoader();
	RESOURCING_API ~ModelLoader();
	RESOURCING_API Resource* LoadResource( const FileContent& fileContent ) override;
	RESOURCING_API void LoadDefaultMaterial();
private:
	void LoadMeshes(ModelResource& model, const aiScene* scene);
	void LoadMaterials(ModelResource& model, const aiScene* scene);

	TextureResource*	m_DefaultAlbedo;
	TextureResource*	m_DefaultNormal;
	TextureResource*	m_DefaultRoughness;
	TextureResource*	m_DefaultMetal;
	bool				m_HasDefault = false;
};
