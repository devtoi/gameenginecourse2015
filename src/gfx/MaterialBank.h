#pragma once
#define g_MaterialBank MaterialBank::GetInstance()
#include <map>
#include <vector>
#include "Texture.h"
#include "GFXLibraryDefine.h"
#include <resourcing/resource/TextureResource.h>
struct aiScene;
typedef int TextureHandle;
namespace gfx {
struct Model;
class Material;
class MaterialBank {
  public:
	~MaterialBank();
 	GFX_API static MaterialBank& GetInstance();
	void Initialize();
	void LoadMaterials(Model& model, std::string filename, const aiScene* scene);
	void ClearMaterials();
	Material* GetMaterial(int matIndex);
	Material* GetMaterial(const std::string& name);
	GFX_API TextureHandle LoadTexture(const char* filename, TextureType type);
	GFX_API Texture* GetTexture(TextureHandle handle);
	int GetMaterialCount() {
		return (int)m_Materials.size();
	}

  private:
	MaterialBank();
	int									m_Numerator = 0;
	std::vector<Material*>				m_Materials;
	std::map<std::string, Material*>	m_MatMap;
	std::vector<Texture*>				m_Textures;
	TextureResource*					m_DefaultAlbedo;
	TextureResource*					m_DefaultNormal;
	TextureResource*					m_DefaultRoughness;
	TextureResource*					m_DefaultMetal;

};
}
