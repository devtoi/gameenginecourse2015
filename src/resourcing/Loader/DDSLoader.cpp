#include "DDSLoader.h"
#include "DDS.h"
#include <stdio.h>
#include <assert.h>
#include <glm/glm.hpp>
#include <gfx/Texture.h>
DDSLoader::DDSLoader() {
}

DDSLoader::~DDSLoader() {

}

Resource* DDSLoader::LoadResource() {
	return nullptr;
}

void DDSLoader::SetWindow(SDL_Window* window) {
	m_Window = window;
}

void DDSLoader::LoadCompleteDDS(const char* filename) {
	FILE* file = fopen(filename, "r");
	DDS_header header;
	unsigned int w = 0;
	unsigned int h = 0;
	unsigned int mipCount = 0;
	bool hasMips = false;
	GLuint texture = 0;
	fread(&header, sizeof(header), 1, file);
	assert(header.dwMagic == DDS_MAGIC);
	assert(header.dwSize == 124);

	w = header.dwWidth;
	h = header.dwHeight;
	mipCount = (header.dwFlags & DDSD_MIPMAPCOUNT) ? header.dwMipMapCount : 1;

	DdsLoadInfo * li;
	if (PF_IS_DXT1(header.sPixelFormat)) {
		li = &loadInfoDXT1;
	}else if (PF_IS_DXT3(header.sPixelFormat)) {
		li = &loadInfoDXT3;
	}else if (PF_IS_DXT5(header.sPixelFormat)) {
		li = &loadInfoDXT5;
	}else if (PF_IS_BGRA8(header.sPixelFormat)) {
		li = &loadInfoBGRA8;
	}else if (PF_IS_BGR8(header.sPixelFormat)) {
		li = &loadInfoBGR8;
	}else if (PF_IS_BGR5A1(header.sPixelFormat)) {
		li = &loadInfoBGR5A1;
	}else if (PF_IS_BGR565(header.sPixelFormat)) {
		li = &loadInfoBGR565;
	}else if (PF_IS_INDEX8(header.sPixelFormat)) {
		li = &loadInfoIndex8;
	}else {
		//unknown format
	}
	if (mipCount > 1)
		hasMips = true;
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (li->compressed) {
		size_t size = glm::max(li->divSize, w) / li->divSize * glm::max(li->divSize, h) / li->divSize * li->blockBytes;
		assert(size == header.dwPitchOrLinearSize);
		assert(header.dwFlags & DDSD_LINEARSIZE);
		unsigned char* buffer = (unsigned char*)malloc(size * 2);
		if (!buffer)
			return;
		glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)SDL_GL_GetProcAddress("glCompressedTexImage2D");
		fread(buffer, 1, size * 2, file);
		size_t offset = 0;
		for (int i = 0; i < mipCount; i++) {
			glCompressedTexImage2D(GL_TEXTURE_2D, i, li->internalFormat, w, h, 0, size, buffer + offset);
			offset += size;
			w = w >> 1;
			h = h >> 1;
			size = glm::max(li->divSize, w) / li->divSize * glm::max(li->divSize, h) / li->divSize * li->blockBytes;
		}
		free(buffer);
	} else {
		if (li->swap)
			glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
		size_t size = w * h * li->blockBytes;
		unsigned char* buffer = (unsigned char*)malloc(size * 2);
		fread(buffer, 1, size * 2, file);
		size_t offset = 0;
		for (int i = 0; i < mipCount; i++) {
			glTexImage2D(GL_TEXTURE_2D, i, li->internalFormat, w, h, 0, li->externalFormat, li->type, buffer + offset);
			offset += size;
			w = w >> 1;
			h = h >> 1;
			size = w * h * li->blockBytes;
		}
		glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
		free(buffer);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount - 1);
	gfx::Texture* tex = new gfx::Texture(texture, gfx::TEXTURE_COLOR);
	fclose(file);
}