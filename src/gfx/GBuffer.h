/**************************************************
Zlib Copyright 2015 Henrik Johansson
***************************************************/

#pragma once
#include <GL/glew.h>
namespace gfx {
enum GBUFFER_TEX {
	ALBEDO24,
	NORMAL16,
	DEPTH_SENCIL32,
	ROUGHNESS8_METAL8,
	COLLECTIVE24,
	NUMBER_OF_GBUFFERS
};

class GBuffer {
  public:
	GBuffer();
	~GBuffer();
	void Initialize(int width, int height);
	void ApplyGeometryStage();
	void ApplyDecalMode();
	void ApplyLightingStage();
	void ApplyPostProcessStage();
	void ClearScreen();
	void Resize(int width, int height);
	GLuint GetTexture(GBUFFER_TEX texture) const;
	GLuint GetFrameBuffer() const;
  private:
	GLuint m_BufferTex[GBUFFER_TEX::NUMBER_OF_GBUFFERS];
	GLuint m_FBO = 0;
	int m_Width;
	int m_Height;
};
};
