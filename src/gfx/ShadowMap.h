#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
namespace gfx {
	class RenderQueue;
	struct Light;
	class ShadowMap {
	public:
		ShadowMap();
		ShadowMap( int size );
		~ShadowMap();
		void Init ();
		void Render ( RenderQueue* rq );
		void SetLight ( const Light& light );
		glm::mat4 GetLightMatrix()const;

		const GLuint GetTexture() const {
			return m_Texture;
		} 

	private:
		GLsizei		 m_Size = 4096;
		GLuint		 m_Texture	   = 0;
		GLuint		 m_FrameBuffer = 0;
		glm::mat4	 m_View;
		glm::mat4	 m_Proj;
		float		 m_Far	= 200.0f;
		float		 m_Near = 1.0f;
		unsigned int m_Shader;
	};
}
