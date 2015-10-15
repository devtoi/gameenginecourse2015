#pragma once
#include <GL/glew.h>

namespace gfx {
	class RenderQueue;
	class GBuffer;
	class TransparencyProgram {
	public:
		TransparencyProgram();
		~TransparencyProgram();
		void Initialize(int width, int height);
		void Render(RenderQueue const* rq, GBuffer const * gbuffer);
		GLuint GetFrameBuffer() const;
	private:
		unsigned int m_GeometryShader;
		unsigned int m_FinalShader;
		GLuint	m_FrameBuffer;
		GLuint	m_AcumTexture;
		GLuint	m_RevealageTexture;
		int		m_Width;
		int		m_Height;
	};
}
