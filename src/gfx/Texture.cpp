#include "Texture.h"
#include <Soil2/SOIL2.h>
#include <glm/glm.hpp>
using namespace gfx;

Texture::Texture() {
	m_Loaded = false;
	m_Width = 0;
	m_Height = 0;
	m_Channels = 0;
	m_Filename = "";
	m_Handle = 0;
}

Texture::Texture(GLuint handle, TextureType type) {
	if (handle == 0 || !glIsTexture(handle))
		return;
	m_Loaded = true;
	glBindTexture(GL_TEXTURE_2D, handle);
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_Width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_Height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_Channels = (type == TEXTURE_COLOR) ? 4 : 1;
	m_Type = type;
	m_HeighestLoadedMip = 0;
	m_MaxMip = log2(glm::max(m_Height, m_Width));
	m_Filename = "";
	m_Handle = handle;
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_Handle);
}

bool Texture::Init(const char* Filename, TextureType type) {
	m_Filename = std::string(Filename);
	m_Type = type;

	if (type == TEXTURE_COLOR || type == TEXTURE_GREYSCALE) {
		int forceChannel = type == TEXTURE_COLOR ? 4 : 1;
		m_Handle = SOIL_load_OGL_texture(Filename, forceChannel, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_GL_MIPMAPS);
		if ( !glIsTexture( m_Handle ) || m_Handle == 0 ) {
			//Logger::Log( pString( "Failed to load texture: " ) + Filename, "Texture", LogSeverity::ERROR_MSG );
			return false;
		}
		m_Channels = forceChannel;
		glBindTexture(GL_TEXTURE_2D, m_Handle);
		//glGenerateMipmap(GL_TEXTURE_2D);
		GLfloat fLargest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest); //TODOHJ: Read from config

		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_Width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		m_MaxMip = log2(glm::max(m_Height, m_Width));
	} else if (type == TEXTURE_CUBE) {
		m_Handle = SOIL_load_OGL_single_cubemap(Filename, SOIL_DDS_CUBEMAP_FACE_ORDER, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_GL_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT | SOIL_FLAG_COMPRESS_TO_DXT);
		if ( !glIsTexture( m_Handle ) ) {
			printf("Failed to load texture %s\n", Filename);
			return false;
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	//Logger::Log( pString( "Loaded texture: " ) + Filename, "Texture", LogSeverity::DEBUG_MSG );
	return true;
}

void Texture::InitWithData(int width, int height, int channels, void* data) {
	m_Width = width;
	m_Height = height;
	m_Type = (channels == 4) ? TEXTURE_COLOR : TEXTURE_GREYSCALE;

	glGenTextures(1, &m_Handle);
	glBindTexture(GL_TEXTURE_2D, m_Handle);
	glTexImage2D(GL_TEXTURE_2D, 0, m_Type == TEXTURE_COLOR ? GL_RGBA8 : GL_R8, width, height, 0, m_Type == TEXTURE_COLOR ? GL_BGRA : GL_R, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_HeighestLoadedMip = 0;
	m_MaxMip = log2(glm::max(m_Height, m_Width));
}

void Texture::InitWithoutData(int width, int height, int channels) {
	m_Width = width;
	m_Height = height;
	m_Type = (channels == 4) ? TEXTURE_COLOR : TEXTURE_GREYSCALE;
	m_MaxMip = log2(glm::max(m_Height, m_Width));
	glGenTextures(1, &m_Handle);
	glBindTexture(GL_TEXTURE_2D, m_Handle);
	unsigned int w = m_Width;
	unsigned int h = m_Height;
	GLuint pbo;
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
	unsigned char* buffer = new unsigned char[m_Width * m_Height * 4];
	memset(buffer, 0xFFU, m_Width * m_Height * 4);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_Width * m_Height * 4, buffer, GL_STATIC_DRAW);
	//generate all mips
	glTexImage2D(GL_TEXTURE_2D, 0, m_Type == TEXTURE_COLOR ? GL_RGBA8 : GL_R8, w, h, 0, m_Type == TEXTURE_COLOR ? GL_BGRA : GL_R, GL_UNSIGNED_BYTE, 0);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
}

void Texture::UpdateMipLevel(int level, void* data) {
	if (level > m_MaxMip) {
		return;
	}
	glBindTexture(GL_TEXTURE_2D, m_Handle);
	int mipW = (unsigned)m_Width >> level;
	int mipH = (unsigned)m_Height >> level;
	glTexImage2D(GL_TEXTURE_2D, level, m_Type == TEXTURE_COLOR ? GL_RGBA8 : GL_R8, mipW, mipH, 0, m_Type == TEXTURE_COLOR ? GL_BGRA : GL_R, GL_UNSIGNED_BYTE, data);
	m_HeighestLoadedMip = (m_HeighestLoadedMip < level) ? m_HeighestLoadedMip : level;
}

GLuint Texture::GetHandle() {
	return m_Handle;
}

void Texture::Apply(GLuint location, int index) {
	glUniform1i(location, index);
	glActiveTexture(GL_TEXTURE0 + index);
	if (m_Type == TEXTURE_COLOR || m_Type == TEXTURE_GREYSCALE)
		glBindTexture(GL_TEXTURE_2D, m_Handle);
	else
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle);
}

bool Texture::GetLoaded() {
	return m_Loaded;
}

void Texture::SetFilename(const char* filename) {
	m_Filename = std::string(filename);
}

std::string Texture::GetFilename() {
	return m_Filename;
}

void Texture::Resize(int width, int height) {
	glBindTexture(GL_TEXTURE_2D, m_Handle);
	GLint intFormat;
	m_Type == TEXTURE_COLOR ? intFormat = GL_RGBA : intFormat = GL_RED;
	glTexStorage2D(GL_TEXTURE_2D, 1, intFormat, width, height);
	glBindTexture(GL_TEXTURE_2D, 0);
}
