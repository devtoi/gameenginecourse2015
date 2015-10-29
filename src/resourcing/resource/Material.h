#pragma once
#include <resourcing/resource/TextureResource.h>
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

	size_t GetAlbedoTexture()					{
		return m_AlbedoTexture;
	}
	size_t GetNormalTexture()					{
		return m_NormalTexture;
	}
	size_t GetRoughnessTexture()				{
		return m_RoughnessTexture;
	}
	size_t GetMetalTexture()					{
		return m_MetalTexture;
	}
	//Setters
	void SetName(std::string name)						{
		m_Name = name;
	}
	void SetAlbedoTexture	(size_t tex)		{
		m_AlbedoTexture = tex;
	}
	void SetNormalTexture	(size_t tex)		{
		m_NormalTexture = tex;
	}
	void SetRoughnessTexture(size_t tex)		{
		m_RoughnessTexture = tex;
	}
	void SetMetalTexture	(size_t tex)		{
		m_MetalTexture = tex;
	}
  private:
	size_t m_AlbedoTexture		= 0;
	size_t m_NormalTexture		= 0;
	size_t m_RoughnessTexture	= 0;
	size_t m_MetalTexture		= 0;
	std::string m_Name = "";
};
