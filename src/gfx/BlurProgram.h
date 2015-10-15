#pragma once
#include <GL/glew.h>
namespace gfx {

	class BlurProgram{
	public:
		BlurProgram();
		~BlurProgram();
		void Initlialize();
		void SetTargetTexture(GLuint texture, float downScale);
		void Render();
		GLuint GetBluredTexture();
	private:
		unsigned int m_ShaderProgram;
		GLuint m_Texture;
		GLuint m_Texture2;
		GLuint m_Target;
		int m_Width;
		int m_Height;
		int m_TargetWidth;
		int m_TargetHeight;
		
	};
}
