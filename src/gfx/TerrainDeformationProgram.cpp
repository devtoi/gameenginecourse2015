#include "TerrainDeformationProgram.h"
#include "RenderQueue.h"
#include "TerrainManager.h"
#include "ShaderBank.h"
#include "FrameBuffer.h"

using namespace gfx;

TerrainDeformationProgram::TerrainDeformationProgram(){

}

TerrainDeformationProgram::~TerrainDeformationProgram(){
	glDeleteTextures(1, &m_BrushTexture);
	delete m_FrameBuffer;
	delete m_BrushTex;
}

void TerrainDeformationProgram::Initialize(){
	m_FrameBuffer = new FrameBuffer();
	m_FrameBuffer->Init();

	glGenTextures(1, &m_BrushTexture);
	glBindTexture(GL_TEXTURE_2D, m_BrushTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_BRUSH_RES, m_BRUSH_RES, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	//fill in brush
	m_BrushGenProgram = g_ShaderBank.LoadShaderProgram("../../../shader/BrushTexGenProgram.glsl");
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_BrushGenProgram);
	prog->Apply();
	prog->SetUniformFloat("hardness", 1.0f);
	glBindImageTexture(0, m_BrushTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	const int WORK_GROUP_SIZE = 32;
	GLuint WorkGroupSize = GLuint((m_BRUSH_RES + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
	glDispatchCompute(WorkGroupSize, WorkGroupSize, 1);
	
	m_DrawProgram = g_ShaderBank.LoadShaderProgram("../../../shader/SpriteShader.glsl");
	m_BrushTex = new Texture();
	m_BrushTex->Init("../../../binasset/texture/brush.png", TEXTURE_COLOR);

	m_FilterProgram = g_ShaderBank.LoadShaderProgram("../../../shader/TerrainDeformFilter.glsl");
}

void TerrainDeformationProgram::Render(RenderQueue* rq){
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_DrawProgram);
	prog->Apply();
	
	glBindVertexArray(0);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE); //additive
	for (auto& td : rq->GetDeformations()){
		Terrain* terrain = g_TerrainManager.GetTerrainFromHandle(td.Terrain);
		glm::vec2 textureSize = terrain->GetHeightHighResSize();

		m_FrameBuffer->SetTexture(terrain->GetHeightMap(), textureSize.x, textureSize.y);
		m_FrameBuffer->Apply();
		glViewport(0, 0, textureSize.x, textureSize.y);
		//prog->SetUniformTextureHandle("g_Texture", m_BrushTexture, 0);
		m_BrushTex->Apply(prog->FetchUniform("g_Texture"), 0);
		prog->SetUniformVec4("g_Pos", glm::vec4(td.Position.x , 1.0f - td.Position.y, 0, 0));
		prog->SetUniformVec4("g_Size", glm::vec4(td.Size, 1, 1));
		glm::vec3 dir = td.Up ? glm::vec3(1) : glm::vec3(0);
		prog->SetUniformVec4("g_Color", glm::vec4(dir, td.Strength));
		prog->SetUniformBool("g_GreyScale", false);
		glDrawArrays(GL_POINTS, 0, 1);
		//filter area
		glm::vec4 pixelArea = glm::vec4((td.Position * textureSize) - glm::vec2(-2), (td.Size * textureSize) + glm::vec2(4));
		const int WORK_GROUP_SIZE = 32;
		GLuint WorkGroupSizeX = GLuint((pixelArea.z + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));
		GLuint WorkGroupSizeY = GLuint((pixelArea.w + WORK_GROUP_SIZE - 1) / float(WORK_GROUP_SIZE));

		ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle(m_FilterProgram);
		prog->Apply();
		prog->SetUniformVec4("g_PixelArea", pixelArea);
		glBindImageTexture(0, terrain->GetHeightMap(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
		glDispatchCompute(WorkGroupSizeX, WorkGroupSizeY, 1);
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
}