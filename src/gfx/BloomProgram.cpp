#include "BloomProgram.h"
#include "ShaderBank.h"
gfx::BloomProgram::BloomProgram(){
}

gfx::BloomProgram::~BloomProgram(){
	delete m_BlurProgram;
}

void gfx::BloomProgram::Initialize(){

	m_Height = 1;
	m_Width = 1;

	glGenTextures(1, &m_BloomTexture);
	glBindTexture(GL_TEXTURE_2D, m_BloomTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_BrightPassShader = g_ShaderBank.LoadShaderProgram("../../../shader/BrightPass.glsl");
	m_BloomShader = g_ShaderBank.LoadShaderProgram("../../../shader/Bloom.glsl");

	m_BlurProgram = new BlurProgram();
	m_BlurProgram->Initlialize();
}

void gfx::BloomProgram::Render(){
	//Do brightpass
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_BrightPassShader);
	prog->Apply();
	const int WORK_GROUP_SIZE = 32;
	GLuint WorkGroupSizeX = GLuint((m_Width + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
	GLuint WorkGroupSizeY = GLuint((m_Height + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
	prog->SetUniformTextureHandle("g_Image", m_TargetTex, 1);
	prog->SetUniformVec2("g_ScreenSize", glm::vec2(m_Width, m_Height));
	prog->SetUniformFloat("g_Threshold", 0.4f);
	prog->SetUniformFloat("g_Smoothness", 0.8f);
	glBindImageTexture(0, m_BloomTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glDispatchCompute(WorkGroupSizeX, WorkGroupSizeY, 1);
	//blur result
	m_BlurProgram->Render();
	//add to final target
	prog = g_ShaderBank.GetProgramFromHandle(m_BloomShader);
	prog->Apply();
	prog->SetUniformTextureHandle("g_BluredBloomTex", m_BlurProgram->GetBluredTexture(), 1);
	prog->SetUniformTextureHandle("g_ColorBuffer", m_TargetTex, 2);
	prog->SetUniformVec2("g_ScreenSize", glm::vec2(m_Width, m_Height));
	glBindImageTexture(0, m_BloomTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glDispatchCompute(WorkGroupSizeX, WorkGroupSizeY, 1);

}

GLuint gfx::BloomProgram::GetFinalTexture(){
	return m_BloomTexture;
}

void gfx::BloomProgram::SetTargetTexture(GLuint targetTex){
	m_TargetTex = targetTex;
	//get size
	glBindTexture(GL_TEXTURE_2D, m_TargetTex);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_Width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_Height);
	//resize texture
	glBindTexture(GL_TEXTURE_2D, m_BloomTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	m_BlurProgram->SetTargetTexture(m_BloomTexture, 0.5f);
}
