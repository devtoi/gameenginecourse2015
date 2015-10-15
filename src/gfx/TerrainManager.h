#pragma once
#include "GFXLibraryDefine.h"
#include "Terrain.h"
#include "VertexBuffer.h"
#include <vector>
#include <unordered_map>
namespace gfx {
#define g_TerrainManager TerrainManager::GetInstance()
	typedef unsigned short TerrainHandle;
	class RenderQueue;
	struct CameraData;

	class TerrainManager {
	public:
		GFX_API static TerrainManager& GetInstance ();
		GFX_API ~TerrainManager();
		GFX_API void		  Init ();
		GFX_API TerrainHandle LoadTerrain ( const char* filename );
		GFX_API void UnloadTerrain( TerrainHandle );

		GFX_API Terrain* GetTerrainFromHandle ( TerrainHandle handle );
		void			 RenderTerrains ( RenderQueue* rq, const CameraData& camera );

	private:
		TerrainManager();
		TerrainHandle m_Numerator = 1;
		unsigned int  m_ShaderProgram;
		VertexBuffer  m_VertexBuffer;
		GLuint		  m_IndexBuffer;
		std::unordered_map<TerrainHandle, Terrain*> m_Terrains;

		const int m_VertexCount = 16;
		const int m_IndexCount	= ( m_VertexCount - 1 ) * ( m_VertexCount - 1 ) * 6;
	};
}
