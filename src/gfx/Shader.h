#pragma once
#include <GL/glew.h>
#include <fstream>
namespace gfx {
class Shader {
  private:
	GLuint m_Handle;
	std::string m_Path;
	GLenum m_Type;
  public:
	Shader();
	~Shader();
	bool CreateFromFile( const std::string& Filename, GLenum ShaderType, bool print );
	bool CreateFromString( std::string ShaderCode, GLenum ShaderType, const std::string& filename, bool print );
	GLuint  GetHandle();
	std::string GetDir( std::string filename );
	std::string LoadText( const std::string& filename );
	void Clear();
	void Recompile();
};
}