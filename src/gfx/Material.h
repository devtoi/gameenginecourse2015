#pragma once
#include <resourcing/resource/TextureResource.h>
namespace gfx {
typedef int TextureHandle;
class Material {
  public:
	Material() { };
	~Material() {
	};
	//Getters
	std::string& GetName() {
		return m_Name;
	}

	TextureResource* GetAlbedoTexture()					{
		return m_AlbedoTexture;
	}
	TextureResource* GetNormalTexture()					{
		return m_NormalTexture;
	}
	TextureResource* GetRoughnessTexture()					{
		return m_RoughnessTexture;
	}
	TextureResource* GetMetalTexture()						{
		return m_MetalTexture;
	}
	//Setters
	void SetName(std::string name)						{
		m_Name = name;
	}
	void SetAlbedoTexture	(TextureResource* tex)		{
		m_AlbedoTexture = tex;
	}
	void SetNormalTexture	(TextureResource* tex)		{
		m_NormalTexture = tex;
	}
	void SetRoughnessTexture(TextureResource* tex)		{
		m_RoughnessTexture = tex;
	}
	void SetMetalTexture	( TextureResource* tex)		{
		m_MetalTexture = tex;
	}
  private:
	TextureResource* m_AlbedoTexture	= nullptr;
	TextureResource* m_NormalTexture	= nullptr;
	TextureResource* m_RoughnessTexture = nullptr;
	TextureResource* m_MetalTexture		= nullptr;
	std::string m_Name = "";
};
}
