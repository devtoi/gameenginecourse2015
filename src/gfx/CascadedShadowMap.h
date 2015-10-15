#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#define PI 3.14159265359f
namespace gfx{
	class RenderQueue;
	struct Light;
	struct CameraData;
	class CascadedShadowMap{
	public:
		CascadedShadowMap();
		~CascadedShadowMap();
		void Initialize();
		void Render(RenderQueue* rq);
		void SetLight(const Light& l);
		void SplitFrustrums();
		glm::mat4* GetLightMatrices(){
			return m_LightMatrices;
		}
		glm::vec4  GetNormalizedFarPlanes(){
			return m_NormalizedFarPlanes;
		}
		GLuint GetTexture(){
			return m_TextureArray;
		}
	private:
		void Update(RenderQueue* rq);
		
		void FrustrumBoundingBoxLightViewSpace(float near, float far, glm::vec4& minOut, glm::vec4& maxOut, const CameraData& cd);
	private:
		const int m_SPLITS = 4;
		const float m_FRUSTRUM_SPLIT_CORRECTION = 0.8f;
		const float m_LIGHT_DISTANCE = 100.0f;
		GLuint m_TextureArray;
		GLuint m_FrameBuffer;
		GLsizei m_Width;
		GLsizei m_Height;
		float m_Near = 0.1f;
		float m_Far = 100.0f;
		unsigned int m_frustumSegmentCount = 4;
		float m_Fov = PI * 0.5f;
		unsigned int m_ShaderProg;
		glm::mat4 m_LightView;
		glm::mat4 m_LightProj;
		glm::mat4 m_LightMatrices[4];
		glm::vec4 m_FarPlanes;
		glm::vec4 m_NormalizedFarPlanes;
		glm::vec4 m_LightViewports[4];
		glm::vec2 m_ViewportDims;
	};
}
