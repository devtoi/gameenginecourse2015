#include "TextureLoader.h"
#include <Soil2/SOIL2.h>
#include <utility/Logger.h>
#include "../resource/TextureResource.h"

TextureLoader::TextureLoader() {

}

TextureLoader::~TextureLoader() {

}

Resource* TextureLoader::LoadResource(const FileContent& fileContent) {
        GLuint texture = SOIL_load_OGL_texture_from_memory((unsigned char*)fileContent.Content,fileContent.Size, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_GL_MIPMAPS);
		if (!glIsTexture(texture) || texture == 0) {
			Logger::Log( pString( "Failed to load texture" ), "Texture", LogSeverity::ERROR_MSG );
			return nullptr;
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		GLfloat fLargest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest); //TODOHJ: Read from config

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		TextureResource* tex = new TextureResource(texture, fileContent.Size);
		tex->SetReady();
		return tex;
}
