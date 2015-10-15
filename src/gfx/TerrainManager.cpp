#include "TerrainManager.h"
#include "RenderQueue.h"
#include "ShaderBank.h"
#include "MaterialBank.h"
#include "Material.h"
#include <glm/gtx/transform.hpp>
using namespace gfx;

TerrainManager::TerrainManager() {
}

TerrainManager::~TerrainManager() {
	// TODOHJ: Terrains should no longer be deleted here becase they should be explicitly deleted. Write logger error if there exist terrains to remove
	for( auto& terrain : m_Terrains ) {
		delete terrain.second;
	}
}

TerrainManager& TerrainManager::GetInstance() {
	static TerrainManager instance;
	return instance;
}

void TerrainManager::Init() {
	//build vbo and ibo
	std::vector<VertexPosNormalTexTangent> vertices;
	VertexPosNormalTexTangent vertex;
	float step = 1.0f / (m_VertexCount - 1);
	for (int z = 0; z < m_VertexCount; z++) {
		for (int x = 0; x < m_VertexCount; x++) {
			vertex.Position = glm::vec4(x * step, 0, z * step, 1);
			vertex.Normal = glm::vec4(0, 1, 0, 0);
			vertex.Tangent = glm::vec4(1, 0, 0, 0);
			vertex.TexCoord = glm::vec4(x * step, z * step, 0, 0);
			vertices.push_back(vertex);
		}
	}
	int sizeX = m_VertexCount;
	std::vector<unsigned int> indices;
	for (int z = 0; z < m_VertexCount - 1; z++) {
		for (int x = 0; x < m_VertexCount - 1; x++) {
			indices.push_back(z * sizeX + x);
			indices.push_back((z + 1) * sizeX + x);
			indices.push_back((z + 1) * sizeX + (x + 1));

			indices.push_back(z * sizeX + x);
			indices.push_back((z + 1) * sizeX + (x + 1));
			indices.push_back(z * sizeX + (x + 1));
		}
	}

	m_VertexBuffer.Init(POS_NORMAL_TEX_TANGENT, &vertices[0], (int)(vertices.size() * sizeof(VertexPosNormalTexTangent)));
	glGenBuffers(1, &m_IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	//set up shader
	m_ShaderProgram = g_ShaderBank.LoadShaderProgram("../../../shader/DeferedTerrain.glsl");
}

TerrainHandle TerrainManager::LoadTerrain(const char* filename) {
	Terrain* terrain = new Terrain();
	terrain->Load(filename);
	m_Terrains.emplace( m_Numerator, terrain );
	return m_Numerator++;
}

void TerrainManager::UnloadTerrain( TerrainHandle terrainHandle ) {
	auto it = m_Terrains.find( terrainHandle );
	if ( it == m_Terrains.end( ) ) {
		// TODOHJ: Log warning about invalid terrain handle
	} else {
		it->second;
		m_Terrains.erase( it );
	}
}

void TerrainManager::RenderTerrains(RenderQueue* rq, const CameraData& camera) {
	m_VertexBuffer.Apply();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_ShaderProgram);
	prog->Apply();
	//set up per frame stuff
	prog->SetUniformMat4("g_ViewProj", camera.ProjView);
	prog->SetUniformVec3("g_CamPos", camera.Position);
	for(auto& it : rq->GetTerrainQueue()) {
		auto terrainIt = m_Terrains.find( it.Terrain );
		if ( terrainIt == m_Terrains.end() ) {
			// TODOHJ: Log error about invalid terrain
			assert( false );
		}
		Terrain* terrain = terrainIt->second;
		//set up per terrain uniforms
		prog->SetUniformMat4("g_World", it.Transform * glm::scale(glm::vec3(it.Size, 1, it.Size)));
		prog->SetUniformVec3("g_Color", it.Color);
		prog->SetUniformVec3("g_TesScaling", glm::vec3(it.Size * 0.25f, it.Size * 0.5f, it.Size * 0.75f));
		prog->SetUniformVec4("g_TesFactor", glm::vec4(8, 4, 2, 1));
		prog->SetUniformFloat("g_Tiling", 2.0f);
		prog->SetUniformFloat("g_Scale", it.HeightScale);
		prog->SetUniformFloat("g_HeightTiling", 1.0f);
		prog->SetUniformFloat("g_TileScale", it.Size / m_VertexCount);
		prog->SetUniformVec2("g_Size", glm::vec2(it.Size));
		//textures
		prog->SetUniformTextureHandle("g_DiffuseTex", g_MaterialBank.GetTexture(terrain->GetAlbedoMap())->GetHandle(), 0);
		prog->SetUniformTextureHandle("g_RoughnessTex", g_MaterialBank.GetTexture(terrain->GetRoughnessMap())->GetHandle(), 1);
		prog->SetUniformTextureHandle("g_HeightMap", terrain->GetHeightMap(), 2);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_PATCHES, m_IndexCount, GL_UNSIGNED_INT, (GLvoid*)0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}


Terrain* TerrainManager::GetTerrainFromHandle(TerrainHandle handle){
	if (handle > 0){
		auto it = m_Terrains.find( handle );
		if ( it == m_Terrains.end( ) ) {
			// TODOHJ: Log about unfound terrain
			return nullptr;
		}
		return it->second;
	} else {
		return nullptr;
	}
}
