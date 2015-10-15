#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "GFXLibraryDefine.h"
#include <string>
namespace gfx {
class Texture;
class VertexBuffer;
class Terrain {
  public:
	GFX_API Terrain();
	GFX_API ~Terrain();
	
	GFX_API void Load(const char* filename);

	float* GetHeightArray() const {
		return m_HeightArray;
	}
	GLuint GetHeightMap() const {
		return m_HeightTexture;
	}
	unsigned int GetAlbedoMap() const {
		return m_ColorTexture;
	}
	unsigned int GetRoughnessMap() const {
		return m_RoughnessTexture;
	}
	const glm::uvec2& GetHeightSize() const {
		return m_HeightSize;
	}
	const glm::uvec2& GetHeightHighResSize() const {
		return m_HeightHighResSize;
	}
	float GetMaxHeightDiff() const {
		return m_MaxHeightDiff;
	}
	const std::string& GetTerrainPath() const {
		return m_TerrainPath;
	}
  private:
	GLuint				m_HeightTexture;
	unsigned int		m_ColorTexture;
	unsigned int		m_RoughnessTexture;
	float*				m_HeightArray = nullptr;
	glm::uvec2			m_HeightSize;
	glm::uvec2			m_HeightHighResSize;
	float				m_MaxHeightDiff;
	std::string 		m_TerrainPath = "";
	const std::string	m_HeightmapFolder = "../../../binasset/heightmap/";
	const std::string	m_TextureFolder = "../../../binasset/texture/";
};
}
