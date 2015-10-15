#pragma once 

#include <GL/glew.h>
#include "BlurProgram.h"
namespace gfx {
	class BloomProgram
	{
	public:
		BloomProgram();
		~BloomProgram();
		void Initialize();
		void Render();
		GLuint GetFinalTexture();
		void SetTargetTexture(GLuint targetTex);
	private:
		BlurProgram*	m_BlurProgram = nullptr;
		unsigned int	m_BrightPassShader;
		unsigned int	m_BloomShader;
		GLuint			m_BloomTexture;
		GLuint			m_TargetTex;
		int				m_Width;
		int				m_Height;
	};
}
