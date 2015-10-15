#pragma once
#include <glm/glm.hpp>
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

	TextureHandle GetAlbedoTexture()					{
		return m_AlbedoTexture;
	}
	TextureHandle GetNormalTexture()					{
		return m_NormalTexture;
	}
	TextureHandle GetRoughnessTexture()					{
		return m_RoughnessTexture;
	}
	TextureHandle GetMetalTexture()						{
		return m_MetalTexture;
	}
	//Setters
	void SetName(std::string name)						{
		m_Name = name;
	}
	void SetAlbedoTexture	( TextureHandle tex)		{
		m_AlbedoTexture = tex;
	}
	void SetNormalTexture	( TextureHandle tex)		{
		m_NormalTexture = tex;
	}
	void SetRoughnessTexture( TextureHandle tex)		{
		m_RoughnessTexture = tex;
	}
	void SetMetalTexture	( TextureHandle tex)		{
		m_MetalTexture = tex;
	}
  private:
	TextureHandle m_AlbedoTexture	= -1;
	TextureHandle m_NormalTexture = -1;
	TextureHandle m_RoughnessTexture = -1;
	TextureHandle m_MetalTexture = -1;
	std::string m_Name = "";
};
}
