#include "SkyProgram.h"
#include "RenderQueue.h"
#include "ShaderBank.h"
#include "ModelBank.h"
#include <glm/gtx/transform.hpp>
using namespace gfx;

SkyProgram::SkyProgram() {

}

SkyProgram::~SkyProgram() {
	delete m_Tex;
}

void SkyProgram::Init() {
	//load Shader
	m_Shader = g_ShaderBank.LoadShaderProgram("../../../shader/DeferedSky.glsl");
	//Load model
	m_Model = g_ModelBank.LoadModel("../../../asset/SkyCube.dae");
}
void SkyProgram::Render(const CameraData& camera) {
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_Shader);
	prog->Apply();
	prog->SetUniformMat4("g_ProjView", camera.ProjView);
	prog->SetUniformMat4("g_World", glm::translate(camera.Position));
	m_Tex->Apply(prog->FetchUniform("g_SkyTex"), 0);
	g_ModelBank.ApplyBuffers();
	Model m = g_ModelBank.FetchModel(m_Model);
	glDrawElements(GL_TRIANGLES, m.Meshes[0].Indices, GL_UNSIGNED_INT,
	               (GLvoid*)(0 + ((m.IndexHandle + m.Meshes[0].IndexBufferOffset) * sizeof(unsigned int))));

	glCullFace(GL_BACK);
}
void SkyProgram::SetSkyTexture(const char* filename) {
	if (m_Tex) {
		delete m_Tex;
		m_Tex = new Texture();
	} else {
		m_Tex = new Texture();
	}
	m_Tex->Init(filename, TEXTURE_CUBE);
}
void SkyProgram::SetSkyTexture(Texture* tex) {
	if (m_Tex) {
		delete m_Tex;
		m_Tex = tex;
	} else {
		m_Tex = tex;
	}
}