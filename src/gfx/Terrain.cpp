#include "Terrain.h"
#include <Soil2/SOIL2.h>
#include "Texture.h"
#include "MaterialBank.h"

using namespace gfx;

Terrain::Terrain() {

}

Terrain::~Terrain() {
	if (m_HeightArray) delete [] m_HeightArray;
}

void Terrain::Load(const char* filename) {
	m_TerrainPath = filename;

	//CallbackConfig* terrainConfig = ConfigManager::GetInstance().GetConfig(filename);

	//rString htFile = terrainConfig->GetString("Heightmap", "test.png");
	//load terrain heightmap
	int width, height, channels;
	unsigned char* image = nullptr;
	image = SOIL_load_image((m_HeightmapFolder).c_str(), &width, &height, &channels, 1);
	if ( image == nullptr ) {
		//Logger::Log( "Terrain failed to load: " + m_HeightmapFolder + htFile, "Terrain", LogSeverity::ERROR_MSG );
		return;
	}
	m_HeightArray = new float[ width * height];
	m_MaxHeightDiff = 0;
	for (int i = 0; i < width * height; ++i) {
		m_HeightArray[i] = (image[i] / 255.0f);
		if (i > 0) {
			float diff = fabs(m_HeightArray[i] - m_HeightArray[i - 1]);
			m_MaxHeightDiff = glm::max(m_MaxHeightDiff, diff);
		}
	}
	m_HeightSize = glm::uvec2(width, height);
	SOIL_free_image_data(image);

	//rString hthrFile = terrainConfig->GetString("HeightmapHighRes", "test_high.png", "A high res version of the heightmap for the graphics");
	//load terrain heightmap
	width, height, channels;
	image = nullptr;
	image = SOIL_load_image((m_HeightmapFolder).c_str(), &width, &height, &channels, 1);
	if (image == nullptr) {
		//Logger::Log("Terrain high res failed to load: " + m_HeightmapFolder + hthrFile, "Terrain", LogSeverity::ERROR_MSG);
		return;
	}
	float* textureArray = new float[ width * height];
	for (int i = 0; i < width * height; ++i) {
		textureArray[i] = (image[i] / 255.0f);
	}

	glGenTextures(1, &m_HeightTexture);
	glBindTexture(GL_TEXTURE_2D, m_HeightTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	m_HeightHighResSize = glm::uvec2(width, height);
	SOIL_free_image_data(image);

	//rString colorFile = terrainConfig->GetString("Albedo", "Grass_default.jpg");
	//m_ColorTexture = g_MaterialBank.LoadTexture((m_TextureFolder + colorFile).c_str(), TEXTURE_COLOR);

	//rString roughnessFile = terrainConfig->GetString("Roughness", "TerrainR.png");
	//m_RoughnessTexture = g_MaterialBank.LoadTexture((m_TextureFolder + roughnessFile).c_str(), TEXTURE_GREYSCALE);

}
