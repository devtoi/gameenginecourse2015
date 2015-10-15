/**************************************************
Zlib Copyright 2015 Henrik Johansson
***************************************************/

#include "GBuffer.h"
using namespace gfx;

GBuffer::GBuffer( ) {
}

GBuffer::~GBuffer( ) {
}

void GBuffer::Initialize( int width, int height ) {
	m_Width = width;
	m_Height = height;
	// Set up Textures
	glDeleteTextures( NUMBER_OF_GBUFFERS, m_BufferTex );
	glGenTextures( NUMBER_OF_GBUFFERS, m_BufferTex );

	glBindTexture( GL_TEXTURE_2D, m_BufferTex[ALBEDO24] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, m_BufferTex[NORMAL16]);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture( GL_TEXTURE_2D, m_BufferTex[DEPTH_SENCIL32] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture( GL_TEXTURE_2D, m_BufferTex[ROUGHNESS8_METAL8] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RG8, m_Width, m_Height, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture( GL_TEXTURE_2D, m_BufferTex[COLLECTIVE24] );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for( int i = 0; i < NUMBER_OF_GBUFFERS; ++i ) {
		if( !glIsTexture( m_BufferTex[i] ) ) {
			//Logger::Log( "Error creating texture for GBuffer", "GBuffer", LogSeverity::ERROR_MSG );
		}
	}
	// Set up framebufffer
	glDeleteFramebuffers( 1, &m_FBO );
	glGenFramebuffers( 1, &m_FBO );
	glBindFramebuffer( GL_FRAMEBUFFER, m_FBO );
	glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_BufferTex[ALBEDO24], 0 );
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_BufferTex[NORMAL16], 0);
	glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_BufferTex[ROUGHNESS8_METAL8], 0 );
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, m_BufferTex[COLLECTIVE24], 0);
	glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_BufferTex[DEPTH_SENCIL32], 0 );

	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == false ) {
		//Logger::Log( "GBuffer error attaching a texture to the framebuffer", "GBuffer", LogSeverity::ERROR_MSG );
	}
}

void GBuffer::ApplyGeometryStage( ) {
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_FBO );
	GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers( 3, drawbuffers );
}

void gfx::GBuffer::ApplyDecalMode()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
	GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);
}

void GBuffer::ApplyLightingStage( ) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
	GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(1, drawbuffers);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GBuffer::ApplyPostProcessStage( ) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void  GBuffer::ClearScreen() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
}

void GBuffer::Resize( int width, int height ) {
	m_Width = width;
	m_Height = height;

	glBindTexture( GL_TEXTURE_2D, m_BufferTex[ALBEDO24] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr );

	glBindTexture(GL_TEXTURE_2D, m_BufferTex[NORMAL16]);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr );

	glBindTexture( GL_TEXTURE_2D, m_BufferTex[DEPTH_SENCIL32] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr );

	glBindTexture( GL_TEXTURE_2D, m_BufferTex[ROUGHNESS8_METAL8] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RG8, m_Width, m_Height, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr );

	glBindTexture( GL_TEXTURE_2D, m_BufferTex[COLLECTIVE24] );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr );
}

GLuint GBuffer::GetTexture( GBUFFER_TEX texture ) const {
	return m_BufferTex[texture];
}

GLuint gfx::GBuffer::GetFrameBuffer() const
{
	return m_FBO;
}
