#include "CascadedShadowMap.h"
#include "RenderQueue.h"
#include "LightEngine.h"
#include "ShaderBank.h"
#include "ModelBank.h"
#include "BufferManager.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace gfx;
#define LIGHT_TEXTURE_SIZE 4096.0f
CascadedShadowMap::CascadedShadowMap() { }

CascadedShadowMap::~CascadedShadowMap() { }

void CascadedShadowMap::Initialize () {
	m_Width = (GLsizei)LIGHT_TEXTURE_SIZE;
	m_Height = (GLsizei)LIGHT_TEXTURE_SIZE;
	glGenFramebuffers ( 1, &m_FrameBuffer );
	glBindFramebuffer ( GL_FRAMEBUFFER, m_FrameBuffer );

	glGenTextures ( 1, &m_TextureArray );
	glBindTexture ( GL_TEXTURE_2D_ARRAY, m_TextureArray );
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_DEPTH_COMPONENT32F, (GLsizei)LIGHT_TEXTURE_SIZE, (GLsizei)LIGHT_TEXTURE_SIZE, m_SPLITS);
	glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,   GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER,   GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
	glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TextureArray, 0 );
	glBindTexture ( GL_TEXTURE_2D_ARRAY, 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );

	m_ShaderProg = g_ShaderBank.LoadShaderProgram ( "../../../shader/CascadedShadowMap.glsl" );

	glGetFloatv ( GL_MAX_VIEWPORT_DIMS, &m_ViewportDims[0] );

	g_BufferManager.BindBufferToProgram ( "ShaderInputs", g_ShaderBank.GetProgramFromHandle ( m_ShaderProg ), 0 );
}

void CascadedShadowMap::Render ( RenderQueue* rq ) {
	ShaderProgram* prog = g_ShaderBank.GetProgramFromHandle ( m_ShaderProg );
	prog->Apply();
	glBindFramebuffer ( GL_FRAMEBUFFER, m_FrameBuffer );

	glEnable ( GL_DEPTH_TEST );
	glEnable (  GL_CULL_FACE );
	glCullFace ( GL_BACK );
	const GLfloat one = 1.0f;
	glClearBufferfv ( GL_DEPTH, 0, &one );
	Update ( rq );
	prog->SetUniformInt ( "g_FrustrumSegmentCount", m_SPLITS );
	prog->SetUniformVec2 ( "g_ShadowMapSize", glm::vec2 ( m_Width, m_Height ) );
	prog->SetUniformVec4 ( "g_Viewports[0]", m_LightViewports[0] );
	prog->SetUniformVec4 ( "g_Viewports[1]", m_LightViewports[1] );
	prog->SetUniformVec4 ( "g_Viewports[2]", m_LightViewports[2] );
	prog->SetUniformVec4 ( "g_Viewports[3]", m_LightViewports[3] );

	glm::mat4 viewProj = m_LightProj * m_LightView;
	prog->SetUniformMat4 ( "g_ViewProj", viewProj );
	unsigned int bufferOffset  = 0;
	unsigned int instanceCount = 0;
	g_ModelBank.ApplyBuffers();
	// for each model to be rendered
	for ( auto& mo : rq->GetModelQueue() ) {
		const Model& model = g_ModelBank.FetchModel ( mo.Model );
		instanceCount = mo.InstanceCount;
		prog->SetUniformUInt ( "g_BufferOffset", bufferOffset );
		// for each mesh
		for ( auto& mesh : model.Meshes ) {
			glDrawElementsInstanced ( GL_TRIANGLES, mesh.Indices, GL_UNSIGNED_INT,
									  ( GLvoid* ) ( 0 + ( ( model.IndexHandle + mesh.IndexBufferOffset ) * sizeof ( unsigned int ) ) ), instanceCount );
		}
		bufferOffset += instanceCount;
	}
}

void CascadedShadowMap::SetLight ( const Light& l ) {
	// calc light matrices
	glm::vec3 pos = glm::normalize(-l.Direction) * m_LIGHT_DISTANCE;

	m_LightView = glm::lookAt ( glm::vec3(100), glm::vec3 ( 0 ), glm::vec3 ( 0, 1.0f, 0 ) );
}

void CascadedShadowMap::Update ( RenderQueue* rq ) {
	// Find a bounding box of whole camera frustum in light view space.
	glm::vec4 frustumMin(std::numeric_limits<float>::max());
	glm::vec4 frustumMax(std::numeric_limits<float>::lowest());
	const CameraData& camera = rq->GetViews().at(0).camera;
	FrustrumBoundingBoxLightViewSpace(m_Near, m_Far, frustumMin, frustumMax, camera);

	// Update light projection matrix to only cover the area viewable by the camera
	m_LightProj = glm::ortho( frustumMin.x, frustumMax.x, frustumMin.y, frustumMax.y, 0.0f, frustumMin.z);

	// Find a bounding box of segment in light view space.
	float nearSegmentPlane = 0.0f;
	for (unsigned int i = 0; i < m_frustumSegmentCount; ++i) {
		glm::vec4 segmentMin(std::numeric_limits<float>::max());
		glm::vec4 segmentMax(std::numeric_limits<float>::lowest());
		FrustrumBoundingBoxLightViewSpace(nearSegmentPlane, m_FarPlanes[i], segmentMin, segmentMax, camera);

		// Update viewports.
		glm::vec2 frustumSize(frustumMax.x - frustumMin.x, frustumMax.y - frustumMin.y);
		const float segmentSizeX = segmentMax.x - segmentMin.x;
		const float segmentSizeY = segmentMax.y - segmentMin.y;
		const float segmentSize = segmentSizeX < segmentSizeY ? segmentSizeY : segmentSizeX;
		const glm::vec2 offsetBottomLeft(segmentMin.x - frustumMin.x, segmentMin.y - frustumMin.y);
		const glm::vec2 offsetSegmentSizeRatio(offsetBottomLeft.x / segmentSize, offsetBottomLeft.y / segmentSize);
		const glm::vec2 frustumSegmentSizeRatio(frustumSize.x / segmentSize, frustumSize.y / segmentSize);

		glm::vec2 pixelOffsetTopLeft(offsetSegmentSizeRatio * LIGHT_TEXTURE_SIZE);
		glm::vec2 pixelFrustumSize(frustumSegmentSizeRatio * LIGHT_TEXTURE_SIZE);

		// Scale factor that helps if frustum size is supposed to be bigger
		// than maximum viewport size.
		glm::vec2 scaleFactor(
			m_ViewportDims[0] < pixelFrustumSize.x ? m_ViewportDims[0] / pixelFrustumSize.x : 1.0f,
			m_ViewportDims[1] < pixelFrustumSize.y ? m_ViewportDims[1] / pixelFrustumSize.y : 1.0f);

		pixelOffsetTopLeft *= scaleFactor;
		pixelFrustumSize *= scaleFactor;

		m_LightViewports[i] = glm::vec4(-pixelOffsetTopLeft.x, -pixelOffsetTopLeft.y, pixelFrustumSize.x, pixelFrustumSize.y);
		glViewportIndexedfv(i, &m_LightViewports[i][0]);

		// Update light view-projection matrices per segment.
		glm::mat4 lightProj;
		lightProj = glm::ortho( segmentMin.x, segmentMin.x + segmentSize, segmentMin.y, segmentMin.y + segmentSize, 0.0f, frustumMin.z);
		glm::mat4 lightScale;
		lightScale = glm::scale(glm::vec3(0.5f * scaleFactor.x, 0.5f * scaleFactor.y, 0.5f));
		glm::mat4 lightBias;
		lightBias = glm::translate(glm::vec3(0.5f * scaleFactor.x, 0.5f * scaleFactor.y, 0.5f));
		m_LightMatrices[i] = lightBias * lightScale * lightProj * m_LightView;

		nearSegmentPlane = m_NormalizedFarPlanes[i];
	}

	// Set remaining viewports to some kind of standard state.
	for (unsigned int i = m_frustumSegmentCount; i < 4; ++i) {
		glViewportIndexedf(i, 0, 0, LIGHT_TEXTURE_SIZE, LIGHT_TEXTURE_SIZE);
	}
}

void CascadedShadowMap::SplitFrustrums() {
	m_LightProj = glm::perspective ( m_Fov, ( 16.0f / 9.0f ), m_Near, m_Far );

	for (unsigned int i = 1; i <= m_frustumSegmentCount; ++i)
	{
		const float distFactor = static_cast<float>(i) / m_frustumSegmentCount;
		const float stdTerm = m_Near * pow(m_Far / m_Near, distFactor);
		const float corrTerm = m_Near + distFactor * (m_Far - m_Near);
		const float viewDepth = m_FRUSTRUM_SPLIT_CORRECTION * stdTerm + (1.0f - m_FRUSTRUM_SPLIT_CORRECTION) * corrTerm;
		m_FarPlanes[i - 1] = viewDepth;
		const glm::vec4 projectedDepth = m_LightProj * glm::vec4(0.0f, 0.0f, -viewDepth, 1.0f);
		// Normalized to [0, 1] depth range.
		m_NormalizedFarPlanes[i - 1] = (projectedDepth.z / projectedDepth.w) * 0.5f + 0.5f;
	}
}

void CascadedShadowMap::FrustrumBoundingBoxLightViewSpace ( float near, float far, glm::vec4& minOut, glm::vec4& maxOut, const CameraData& cd ) {
	glm::vec4 frustumMin(std::numeric_limits<float>::max());
	glm::vec4 frustumMax(std::numeric_limits<float>::lowest());

	const float nearHeight = 2.0f * tan(m_Fov * 0.5f) * near;
	const float nearWidth = nearHeight * static_cast<float>(m_Width) / m_Height;
	const float farHeight = 2.0f * tan(m_Fov * 0.5f) * far;
	const float farWidth = farHeight * static_cast<float>(m_Width) / m_Height;
	const glm::vec4 cameraPos = glm::vec4(cd.Position, 1);
	const glm::mat4 invRot = glm::inverse(cd.View);
	const glm::vec4 viewDir = invRot *  glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	const glm::vec4 upDir = invRot * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	const glm::vec4 rightDir = invRot * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	const glm::vec4 nc = cameraPos + viewDir * near; // near center
	const glm::vec4 fc = cameraPos + viewDir * far; // far center

	// Vertices in a world space.
	glm::vec4 vertices[8] = {
		nc - upDir * nearHeight * 0.5f - rightDir * nearWidth * 0.5f, // nbl (near, bottom, left)
		nc - upDir * nearHeight * 0.5f + rightDir * nearWidth * 0.5f, // nbr
		nc + upDir * nearHeight * 0.5f + rightDir * nearWidth * 0.5f, // ntr
		nc + upDir * nearHeight * 0.5f - rightDir * nearWidth * 0.5f, // ntl
		fc - upDir * farHeight  * 0.5f - rightDir * farWidth * 0.5f, // fbl (far, bottom, left)
		fc - upDir * farHeight  * 0.5f + rightDir * farWidth * 0.5f, // fbr
		fc + upDir * farHeight  * 0.5f + rightDir * farWidth * 0.5f, // ftr
		fc + upDir * farHeight  * 0.5f - rightDir * farWidth * 0.5f, // ftl
	};

	for (unsigned int vertId = 0; vertId < 8; ++vertId) {
		// Light view space.
		vertices[vertId] = m_LightView * vertices[vertId];
		// Update bounding box.
		frustumMin = glm::min(frustumMin, vertices[vertId]);
		frustumMax = glm::max(frustumMax, vertices[vertId]);
	}

	minOut = frustumMin;
	maxOut = frustumMax;
}
