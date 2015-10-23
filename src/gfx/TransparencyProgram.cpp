#include "TransparencyProgram.h"
#include "ShaderBank.h"
#include "RenderQueue.h"
#include "ModelBank.h"
#include "MaterialBank.h"
#include "Material.h"
#include "BufferManager.h"
#include "GBuffer.h"
#include "LightEngine.h"
gfx::TransparencyProgram::TransparencyProgram(){
}

gfx::TransparencyProgram::~TransparencyProgram(){
}

void gfx::TransparencyProgram::Initialize(int width, int height){
	m_Width = width;
	m_Height = height;
	//generate textures
	glGenTextures(1, &m_AcumTexture);
	glBindTexture(GL_TEXTURE_2D, m_AcumTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenTextures(1, &m_RevealageTexture);
	glBindTexture(GL_TEXTURE_2D, m_RevealageTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//create framebuffer
	glGenFramebuffers(1, &m_FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_AcumTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_RevealageTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == false) {
		//Logger::Log("TransparencyProgram error attaching a texture to the framebuffer", "TransparencyProgram", LogSeverity::ERROR_MSG);
	}
	//compile shaders
	m_GeometryShader = g_ShaderBank.LoadShaderProgram("../../../shader/TransparentGeometry.glsl");
	m_FinalShader = g_ShaderBank.LoadShaderProgram("../../../shader/TransparentFinal.glsl");

	g_BufferManager.BindBufferToProgram("TransparentShaderInputs", g_ShaderBank.GetProgramFromHandle(m_GeometryShader), 1);
	g_BufferManager.BindBufferToProgram("LightBuffer", g_ShaderBank.GetProgramFromHandle(m_GeometryShader), 3);
}

void gfx::TransparencyProgram::Render(RenderQueue const * rq, GBuffer const * gbuffer){
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_GeometryShader);
	prog->Apply();
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	glBindFramebuffer(GL_FRAMEBUFFER,m_FrameBuffer);
	//clear rt
	GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDrawBuffers(1, drawbuffers);
	glClear(GL_COLOR_BUFFER_BIT);

	GLenum drawbuffers2[] = { GL_COLOR_ATTACHMENT1 };
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glDrawBuffers(1, drawbuffers2);
	glClear(GL_COLOR_BUFFER_BIT);

	GLenum drawbuffers3[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawbuffers3);
	for (auto& view : rq->GetViews()) {
		glViewport((GLint)view.viewport.x, (GLint)view.viewport.y, (GLsizei)view.viewport.width, (GLsizei)view.viewport.height);
		g_ModelBank.ApplyBuffers();
		// set per frame variables
		prog->SetUniformMat4("g_ViewProj", view.camera.ProjView);
		prog->SetUniformTextureHandle("g_DepthBuffer", gbuffer->GetTexture(GBUFFER_TEX::DEPTH_SENCIL32), 4);
		prog->SetUniformUInt("g_NumDLights", g_LightEngine.GetDirLightCount());
		prog->SetUniformVec3("g_CamPos", view.camera.Position);
		unsigned int bufferOffset = 0;
		unsigned int instanceCount = 0;
		// for each model to be rendered
		for (auto& mo : rq->GetTranparentModelQueue()) {
			const Model& model = g_ModelBank.FetchModel(mo.Model);
			instanceCount = mo.InstanceCount;
			prog->SetUniformUInt("g_BufferOffset", bufferOffset);
			prog->SetUniformFloat("g_Transparency", mo.Transparency);
			prog->SetUniformFloat("g_ZNear", view.camera.Near);
			prog->SetUniformFloat("g_ZFar", view.camera.Far);
			// for each mesh
			for (auto& mesh : model.Meshes) {
				// set textures
				Material* mat = g_MaterialBank.GetMaterial(model.MaterialOffset + mesh.Material);

				//Texture*  albedoTex = mat->GetAlbedoTexture()->GetTexture();
				//Texture*  normalTex = mat->GetNormalTexture()->GetTexture();
				//Texture*  roughnessTex = mat->GetRoughnessTexture()->GetTexture();
				//Texture*  metalTex = mat->GetMetalTexture()->GetTexture();

				prog->SetUniformTextureHandle("g_DiffuseTex",	mat->GetAlbedoTexture()->GetTexture(), 0);
				prog->SetUniformTextureHandle("g_NormalTex",	mat->GetNormalTexture()->GetTexture(), 1);
				prog->SetUniformTextureHandle("g_RoughnessTex", mat->GetRoughnessTexture()->GetTexture(), 2);
				prog->SetUniformTextureHandle("g_MetallicTex",	mat->GetMetalTexture()->GetTexture(), 3);
				glDrawElementsInstanced(GL_TRIANGLES, mesh.Indices, GL_UNSIGNED_INT,
					(GLvoid*)(0 + ((model.IndexHandle + mesh.IndexBufferOffset) * sizeof(unsigned int))), instanceCount);
			}
			bufferOffset += instanceCount;
		}
	}

	prog = g_ShaderBank.GetProgramFromHandle(m_FinalShader);
	prog->Apply();
	prog->SetUniformTextureHandle("acumTexture", m_AcumTexture, 0);
	prog->SetUniformTextureHandle("revealageTexture", m_RevealageTexture, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	glDrawArrays(GL_POINTS, 0, 1);
	glClearColor(0.39f, 0.61f, 0.93f, 1.0f); // cornflower blue
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

GLuint gfx::TransparencyProgram::GetFrameBuffer() const
{
	return m_FrameBuffer;
}
