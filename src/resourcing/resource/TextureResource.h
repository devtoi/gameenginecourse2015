#pragma once

#include "../Resource.h"
#include "../ResourcingLibraryDefine.h"
#include <memory>
#include <GL/glew.h>
class TextureResource : public Resource {
public:
	TextureResource( GLuint tex,  size_t size );
	RESOURCING_API GLuint GetTexture() const;
	void Init(GLuint tex, size_t size);
private:
	GLuint m_Texture = 0;
};
