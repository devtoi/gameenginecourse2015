#pragma once
#include <GL/glew.h>
#include <fstream>
#include "GFXLibraryDefine.h"
namespace gfx {
class Shader {
public:
	GFX_API Shader();
	GFX_API ~Shader();
	GFX_API bool CreateFromFile(const std::string& Filename, GLenum ShaderType, bool print);
	GFX_API bool CreateFromString(std::string ShaderCode, GLenum ShaderType, const std::string& filename, bool print);
	GFX_API GLuint  GetHandle();
	GFX_API std::string GetDir(std::string filename);
	GFX_API std::string LoadText(const std::string& filename);
	GFX_API void Clear();
	GFX_API void Recompile();
  private:
	GLuint m_Handle;
	std::string m_Path;
	GLenum m_Type;

};
}