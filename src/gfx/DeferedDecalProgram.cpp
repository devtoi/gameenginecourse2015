#include "DeferedDecalProgram.h"
#include "ShaderBank.h"
#include "RenderQueue.h"
#include "GBuffer.h"
#include <resourcing/ResourceManager.h>
#include <resourcing/ModelBank.h>
gfx::DeferedDecalProgram::DeferedDecalProgram(){

}

gfx::DeferedDecalProgram::~DeferedDecalProgram(){

}

void gfx::DeferedDecalProgram::Initialize(){
	//Load Shader
	m_ShaderProg = g_ShaderBank.LoadShaderProgram("../../../shader/DeferedDecals.glsl");
	//Load model
	//m_Model = HashResourceName("Model.Cube");
	//g_ResourceManager.AquireResource(m_Model);
}

void gfx::DeferedDecalProgram::Render(RenderQueue* rq, GBuffer* gbuffer){
	gbuffer->ApplyDecalMode();
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_ShaderProg);
	prog->Apply();
	g_ModelBank.ApplyBuffers();
	ModelResource* model = (ModelResource*)g_ResourceManager.GetResourcePointer(m_Model);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	for (auto& view : rq->GetViews()) {
		glViewport((int)view.viewport.x, (int)view.viewport.y, (int)view.viewport.width, (int)view.viewport.height);
		prog->SetUniformMat4("g_ViewProj", view.camera.ProjView);
		prog->SetUniformTextureHandle("g_Depth", gbuffer->GetTexture(GBUFFER_TEX::DEPTH_SENCIL32), 1);
		prog->SetUniformMat4("g_InvViewProj", glm::inverse(view.camera.ProjView));
		prog->SetUniformVec2("g_ViewportSize", glm::vec2(view.viewport.width, view.viewport.height));
		prog->SetUniformVec2("g_ViewportOffset", glm::vec2(view.viewport.x, view.viewport.y));
		for (auto& decal : rq->GetDeferedDecals()) {
			//prog->SetUniformTextureHandle("g_DecalAlbedo", g_MaterialBank.GetTexture(decal.Texture)->GetHandle(), 0);
			prog->SetUniformVec4("g_Tint", decal.Tint);
			prog->SetUniformMat4("g_World", decal.World);
			prog->SetUniformMat4("g_InvWorld", glm::inverse(decal.World));
			
			glDrawElements(GL_TRIANGLES, model->Meshes[0].IndexCount, GL_UNSIGNED_INT,
				(GLvoid*)(0 + ((model->IndexOffset + model->Meshes[0].IndexOffset) * sizeof(unsigned int))));
		}
	}
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}
