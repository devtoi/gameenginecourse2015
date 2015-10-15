#pragma once 
namespace gfx {
	class RenderQueue;
	class FrameBuffer;
	class Texture;
	class TerrainDeformationProgram{
	public:
		TerrainDeformationProgram();
		~TerrainDeformationProgram();
		void Initialize();
		void Render(RenderQueue* rq);
	private:
		unsigned int m_BrushGenProgram;
		unsigned int m_DrawProgram;
		unsigned int m_BrushTexture;
		unsigned int m_FilterProgram;
		FrameBuffer* m_FrameBuffer = nullptr;
		Texture*	 m_BrushTex = nullptr;
		const int m_BRUSH_RES = 256;
	};
}