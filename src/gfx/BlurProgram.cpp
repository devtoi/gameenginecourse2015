#include "BlurProgram.h"
#include "ShaderBank.h"
gfx::BlurProgram::BlurProgram()
{ }

gfx::BlurProgram::~BlurProgram()
{ }

void gfx::BlurProgram::Initlialize(){
	m_Height = 1;
	m_Width = 1;
	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glGenTextures(1, &m_Texture2);
	glBindTexture(GL_TEXTURE_2D, m_Texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	m_ShaderProgram = g_ShaderBank.LoadShaderProgram("../../../shader/GaussianBlur.glsl");
}

void gfx::BlurProgram::SetTargetTexture( GLuint texture, float downScale ){
	m_Target = texture;
	//get size
	glBindTexture(GL_TEXTURE_2D, m_Target);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_TargetWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_TargetHeight);

	m_Width = (int)(m_TargetWidth * downScale);
	m_Height = (int)(m_TargetHeight * downScale);
	//resize texture
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, m_Texture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void gfx::BlurProgram::Render(){
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_ShaderProgram);
	prog->Apply();
	//set target texture sampler to linear mode
	glBindTexture(GL_TEXTURE_2D, m_Target);
	GLint oldMode = 0;
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_MAG_FILTER, &oldMode);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	const int WORK_GROUP_SIZE = 32;
	GLuint WorkGroupSizeX = GLuint((m_Width + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
	GLuint WorkGroupSizeY = GLuint((m_Height + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
	
	//do horisontal pass
	prog->SetUniformBool("g_Horisontal", true);
	prog->SetUniformVec2("g_PixelSize", glm::vec2(1.0f / m_Width, 1.0f / m_Height));
	prog->SetUniformVec2("g_ScreenSize", glm::vec2(m_Width, m_Height));
	prog->SetUniformTextureHandle("g_Image", m_Target, 1);
	glBindImageTexture(0, m_Texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glDispatchCompute(WorkGroupSizeX, WorkGroupSizeY, 1);
	
	//and vertical pass
	prog->SetUniformBool("g_Horisontal", false);
	prog->SetUniformTextureHandle("g_Image", m_Texture, 1);
	glBindImageTexture(0, m_Texture2, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glDispatchCompute(WorkGroupSizeX, WorkGroupSizeY, 1);
	//restore state
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

GLuint gfx::BlurProgram::GetBluredTexture() {
	return m_Texture2;
}

