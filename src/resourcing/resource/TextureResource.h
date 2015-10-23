#pragma once

#include "../Resource.h"
#include <memory>

namespace gfx {
	class Texture;
};

class TextureResource : public Resource {
public:
	TextureResource( std::unique_ptr<gfx::Texture> texture, size_t size );
	gfx::Texture* GetTexture() const;

private:
	std::unique_ptr<gfx::Texture> m_Texture = nullptr;
};
