#include "TextureResource.h"

TextureResource::TextureResource(GLuint tex, size_t size )
	: Resource(), m_Texture( tex ) {
	this->SetSize(size);
}

GLuint TextureResource::GetTexture() const {
	if (!this->IsReady())
		return 0;
	return m_Texture;
}

void TextureResource::Init(GLuint tex, size_t size) {
	m_Texture = tex;
	this->SetSize(size);
}