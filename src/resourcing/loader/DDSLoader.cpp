#include "DDSLoader.h"
#include <cstring>
#include <stdio.h>
#include <assert.h>
#include <glm/glm.hpp>
#include <utility/SerializationUtility.h>
#include <SDL2/SDL.h>
#include "DDS.h"
#include "../resource/TextureResource.h"

DDSLoader::DDSLoader() { }

DDSLoader::~DDSLoader() { }

Resource* DDSLoader::LoadResource( const FileContent& fileContent ) {
	const Byte* content = static_cast<Byte*>( fileContent.Content );
	DDS_header	 header;
	unsigned int w		  = 0;
	unsigned int h		  = 0;
	unsigned int mipCount = 0;
	bool		 hasMips  = false;
	GLuint texture = 0;

	SerializationUtility::CopyAndIncrementSource( &header, content, sizeof( header ) );
	assert( header.dwMagic == DDS_MAGIC );
	assert( header.dwSize == 124 );
	w		 = header.dwWidth;
	h		 = header.dwHeight;
	mipCount = ( header.dwFlags & DDSD_MIPMAPCOUNT ) ? header.dwMipMapCount : 1;

	DdsLoadInfo * li;
	if ( PF_IS_DXT1( header.sPixelFormat ) ) {
		li = &loadInfoDXT1;
	} else if ( PF_IS_DXT3( header.sPixelFormat ) )      {
		li = &loadInfoDXT3;
	} else if ( PF_IS_DXT5( header.sPixelFormat ) )      {
		li = &loadInfoDXT5;
	} else if ( PF_IS_BGRA8( header.sPixelFormat ) )      {
		li = &loadInfoBGRA8;
	} else if ( PF_IS_BGR8( header.sPixelFormat ) )      {
		li = &loadInfoBGR8;
	} else if ( PF_IS_BGR5A1( header.sPixelFormat ) )      {
		li = &loadInfoBGR5A1;
	} else if ( PF_IS_BGR565( header.sPixelFormat ) )      {
		li = &loadInfoBGR565;
	} else if ( PF_IS_INDEX8( header.sPixelFormat ) )      {
		li = &loadInfoIndex8;
	} else  {
		// unknown format
		li = nullptr;
	}
	if ( mipCount > 1 ) {
		hasMips = true;
	}
	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture);
	if ( li->compressed ) {
		size_t size = glm::max( li->divSize, w ) / li->divSize * glm::max( li->divSize, h ) / li->divSize * li->blockBytes;
		assert( size == header.dwPitchOrLinearSize );
		assert( header.dwFlags & DDSD_LINEARSIZE );
		unsigned char* buffer = ( unsigned char* )tMalloc( size * 2 );
		if ( !buffer ) {
			return nullptr;
		}
		glCompressedTexImage2D = ( PFNGLCOMPRESSEDTEXIMAGE2DPROC )SDL_GL_GetProcAddress( "glCompressedTexImage2D" );
		SerializationUtility::CopyAndIncrementSource( buffer, content, size * 2 );
		size_t offset = 0;
		for ( unsigned int i = 0; i < mipCount; i++ ) {
			glCompressedTexImage2D( GL_TEXTURE_2D, i, li->internalFormat, w, h, 0, size, buffer + offset );
			offset += size;
			w		= w >> 1;
			h		= h >> 1;
			size	= glm::max( li->divSize, w ) / li->divSize * glm::max( li->divSize, h ) / li->divSize * li->blockBytes;
		}
		free( buffer );
	} else if ( li->palette ) {
		assert( header.dwFlags & DDSD_PITCH );
		assert( header.sPixelFormat.dwRGBBitCount == 8 );
		size_t size = header.dwPitchOrLinearSize * h;
		assert( size == w * h * li->blockBytes );
		unsigned char * data = ( unsigned char* )tMalloc( size );
		unsigned int	palette[256];
		unsigned int *	unpacked = ( unsigned int* )tMalloc( size * sizeof( unsigned int ) );
		SerializationUtility::CopyAndIncrementSource( palette, content, 4 * 256 );
		for ( unsigned int ix = 0; ix < mipCount; ++ix ) {
			SerializationUtility::CopyAndIncrementSource( data, content, size );
			for ( unsigned int zz = 0; zz < size; ++zz ) {
				unpacked[zz] = palette[data[zz]];
			}
			glPixelStorei( GL_UNPACK_ROW_LENGTH, h );
			glTexImage2D( GL_TEXTURE_2D, ix, li->internalFormat, w, h, 0, li->externalFormat, li->type, unpacked );
			w	 = w >> 1;
			h	 = h >> 1;
			size = w * h * li->blockBytes;
		}
		tFree( data );
		tFree( unpacked );
	} else   {
		if ( li->swap ) {
			glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_TRUE );
		}
		size_t		   size	  = w * h * li->blockBytes;
		for ( unsigned int i = 0; i < mipCount; i++ ) {
			glTexImage2D( GL_TEXTURE_2D, i, li->internalFormat, w, h, 0, li->externalFormat, li->type, content );
			content += size;
			w		= w >> 1;
			h		= h >> 1;
			size	= w * h * li->blockBytes;
		}
		glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
	}
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipCount - 1);
	glBindTexture(GL_TEXTURE_2D, 0);
	TextureResource* tex = new TextureResource(texture,fileContent.Size - sizeof(header));
	tex->SetReady();
    return tex;
}

