#include "ShadowMap.h"
#include "ShaderBank.h"
#include "BufferManager.h"
#include "RenderQueue.h"
#include "Lights.h"
#include <glm/gtx/transform.hpp>
using namespace gfx;
#define LIGHT_DIST 80.0f
ShadowMap::ShadowMap() { }

ShadowMap::ShadowMap( int size ) {
	m_Size = size;
}

ShadowMap::~ShadowMap() { }

void ShadowMap::Init() {

	glGenFramebuffers( 1, &m_FrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FrameBuffer );

	glGenTextures( 1, &m_Texture );
	glBindTexture( GL_TEXTURE_2D, m_Texture );
	glTexStorage2D( GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, m_Size, m_Size );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	 GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,	 GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,		 GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,		 GL_CLAMP_TO_EDGE );

	glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_Texture, 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	m_Shader = g_ShaderBank.LoadShaderProgram( "../../../shader/Shadow.glsl" );

	g_BufferManager.BindBufferToProgram( "ShaderInputs", g_ShaderBank.GetProgramFromHandle( m_Shader ), 0 );
	float size = 5.0f;
	m_Proj = glm::ortho( -size, size, -size, size, m_Near, m_Far );
}

void ShadowMap::Render( RenderQueue* rq ) {
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle( m_Shader );

	prog->Apply();
	glBindFramebuffer( GL_FRAMEBUFFER, m_FrameBuffer );

	glEnable( GL_DEPTH_TEST );
	glEnable(  GL_CULL_FACE );
	glCullFace( GL_BACK);
	const GLfloat one = 1.0f;
	glClearBufferfv( GL_DEPTH, 0, &one );
	glViewport( 0, 0, m_Size, m_Size );
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset( 1.0f, 4.0f );

	//glm::mat4 viewProj = m_Proj * m_View;
	//prog->SetUniformMat4( "g_ViewProj", viewProj );
	//unsigned int bufferOffset  = 0;
	//unsigned int instanceCount = 0;
	//g_ModelBank.ApplyBuffers();
	//// for each model to be rendered
	//for ( auto& mo : rq->GetModelQueue() ) {
	//	const Model& model = g_ModelBank.FetchModel( mo.Model );
	//	instanceCount = mo.InstanceCount;
	//	prog->SetUniformUInt( "g_BufferOffset", bufferOffset );
	//	// for each mesh
	//	for ( auto& mesh : model.Meshes ) {
	//		glDrawElementsInstanced( GL_TRIANGLES, mesh.Indices, GL_UNSIGNED_INT,
	//			( GLvoid* )( 0 + ( ( model.IndexHandle + mesh.IndexBufferOffset ) * sizeof( unsigned int ) ) ), instanceCount );
	//	}
	//	bufferOffset += instanceCount;
	//}
	//glDisable(GL_POLYGON_OFFSET_FILL);
}

glm::mat4 ShadowMap::GetLightMatrix() const {
	return m_Proj * m_View;
}

void ShadowMap::SetLight( const Light& light ) {
	glm::vec3 pos = glm::normalize( -light.Direction ) * LIGHT_DIST;

	m_View = glm::lookAt( pos, glm::vec3( 0 ), glm::vec3( 0, 1, 0 ) );
}