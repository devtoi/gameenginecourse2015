#include "DDSLoader.h"
#include <cstring>
#include <stdio.h>
#include <assert.h>
#include <glm/glm.hpp>
#include <gfx/Texture.h>
#include "DDS.h"
#include "../Resource.h"

DDSLoader::DDSLoader() {
}

DDSLoader::~DDSLoader() {

}

std::unique_ptr<Resource> DDSLoader::LoadResource( const FileContent& fileContent ) {
	struct Derp {
        size_t Size;
		size_t Position;
		void* Content;
		void read( void* ptr, size_t size, size_t count ) {
            if ( Position + size * count > Size ) {
                memcpy( ptr, (void*)( (size_t)Content + Position ), Size - Position );
            } else {
                memcpy( ptr, (void*)( (size_t)Content + Position ), size * count );
            }
			Position += size * count;
		}
	};
    Derp herp{ fileContent.Size, 0, fileContent.Content };
	DDS_header header;
	unsigned int w = 0;
	unsigned int h = 0;
	unsigned int mipCount = 0;
	bool hasMips = false;
	GLuint texture = 0;
	//fread(&header, sizeof(header), 1, file);
	herp.read(&header, sizeof(header), 1 );
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
		if (!buffer) {
			return std::unique_ptr<Resource>( nullptr );
		}
		glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)SDL_GL_GetProcAddress("glCompressedTexImage2D");
		//fread(buffer, 1, size * 2, file);
		herp.read(buffer, 1, size * 2);
		size_t offset = 0;
		for (unsigned int i = 0; i < mipCount; i++) {
			glCompressedTexImage2D(GL_TEXTURE_2D, i, li->internalFormat, w, h, 0, size, buffer + offset);
			offset += size;
			w = w >> 1;
			h = h >> 1;
			size = glm::max(li->divSize, w) / li->divSize * glm::max(li->divSize, h) / li->divSize * li->blockBytes;
		}
		free(buffer);
	} else if (li->palette) {
		assert(header.dwFlags & DDSD_PITCH);
		assert(header.sPixelFormat.dwRGBBitCount == 8);
		size_t size = header.dwPitchOrLinearSize * h;
		assert(size == w * h * li->blockBytes);
		unsigned char * data = (unsigned char *)malloc(size);
		unsigned int palette[256];
		unsigned int * unpacked = (unsigned int *)malloc(size*sizeof(unsigned int));
		//fread(palette, 4, 256, file);
		herp.read(palette, 4, 256);
		for (unsigned int ix = 0; ix < mipCount; ++ix) {
			//fread(data, 1, size, file);
			herp.read(data, 1, size);
			for (unsigned int zz = 0; zz < size; ++zz) {
				unpacked[zz] = palette[data[zz]];
			}
			glPixelStorei(GL_UNPACK_ROW_LENGTH, h);
			glTexImage2D(GL_TEXTURE_2D, ix, li->internalFormat, w, h, 0, li->externalFormat, li->type, unpacked);
			w = w >> 1;
			h = h >> 1;
			size = w * h * li->blockBytes;
		}
		free(data);
		free(unpacked);
	}
	else {
		if (li->swap)
			glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
		size_t size = w * h * li->blockBytes;
		unsigned char* buffer = (unsigned char*)malloc(size * 2);
		//fread(buffer, 1, size * 2, file);
		herp.read(buffer, 1, size * 2);
		size_t offset = 0;
		for (unsigned int i = 0; i < mipCount; i++) {
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
    return std::unique_ptr<Resource>( nullptr );
}

void DDSLoader::SetWindow(SDL_Window* window) {
	m_Window = window;
}

