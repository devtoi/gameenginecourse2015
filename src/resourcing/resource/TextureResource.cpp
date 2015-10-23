#include "TextureResource.h"
#include <gfx/Texture.h>

TextureResource::TextureResource( std::unique_ptr<gfx::Texture> texture, size_t size )
	: Resource( size ), m_Texture( std::move( texture ) ) {
}

gfx::Texture* TextureResource::GetTexture() const {
	return m_Texture.get();
}
