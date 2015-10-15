#include "FrameBuffer.h"
#include "Texture.h"
using namespace gfx;

FrameBuffer::FrameBuffer() {
}

FrameBuffer::~FrameBuffer() {
	m_TargetTexture = nullptr;
}

void FrameBuffer::Init() {
	glGenFramebuffers(1, &m_Handle);
}

void FrameBuffer::Apply() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	glViewport(0, 0, m_Width, m_Height);
}

void FrameBuffer::SetTexture(gfx::Texture* tex) {
	//dont change if the target has not changed
	if (m_TargetTexture == tex)
		return;
	m_TargetTexture = tex;

	m_Width = (GLuint)m_TargetTexture->GetWidth();
	m_Height = (GLuint)m_TargetTexture->GetHeight();
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TargetTexture->GetHandle(), 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status) {
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT :
		printf("FBO error, incomplete attachment\n");
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::SetTexture(GLuint tex, int width, int height) {
	m_TargetTexture = nullptr;

	m_Width = width;
	m_Height = height;
	glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status) {
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		printf("FBO error, incomplete attachment\n");
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

gfx::Texture* FrameBuffer::GetTexture() {
	return m_TargetTexture;
}