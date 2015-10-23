#include "TextureResource.h"

TextureResource::TextureResource(GLuint tex, size_t size )
	: Resource( size ), m_Texture( tex ) {
}

GLuint TextureResource::GetTexture() const {
	return m_Texture;
}
