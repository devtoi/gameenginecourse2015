#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <string>

typedef int TextureHandle;
namespace gfx {
enum TextureType {
	TEXTURE_COLOR,
	TEXTURE_GREYSCALE,
	TEXTURE_CUBE
};

class Texture {
  public:
	Texture();
	~Texture();

	bool Init( const char* Filename, TextureType type);
	void InitWithData(int width, int height, int channels, void* data);
	void InitWithoutData(int width, int height, int channels);
	void UpdateMipLevel(int level, void* data);
	GLuint GetHandle();
	void Apply( GLuint location, int index );
	bool GetLoaded();
	void SetFilename( const char* filename );
	std::string GetFilename();

	float GetWidth() { return (float)m_Width; }
	float GetHeight() { return (float)m_Height; }
	int GetChannels() { return m_Channels; }
	int GetHeighestMip() { return m_HeighestLoadedMip; }
	void Resize( int width, int height );
  private:
	GLuint m_Handle;
	int m_Width, m_Height, m_Channels;
	int m_HeighestLoadedMip;
	int m_MaxMip;
	bool m_Loaded;
	std::string m_Filename;
	TextureType m_Type;
};
}
