#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "ShaderDefineContainer.h"
#include "Texture.h"
#include "ShaderProgramBinary.h"
#include "GFXLibraryDefine.h"
#include <vector>
#include <map>

namespace gfx {
class ShaderProgram {
  public:
	GFX_API ShaderProgram ( void );
	GFX_API ShaderProgram ( std::vector<Shader*> Shaders );
	GFX_API ~ShaderProgram ( void );
	GFX_API bool Init ( std::vector<Shader*> Shaders, bool print );
	GFX_API GLuint GetHandle();
	GFX_API std::string& GetFilename();
	GFX_API void Apply();
	GFX_API bool Validate();
	GFX_API GLuint GetUniformLocationByName ( const std::string& name );
	GFX_API bool LoadCompleteShaderProgramFromFile ( const std::string& filename, bool print );
	GFX_API bool LoadCompleteShaderProgramFromFile ( const std::string& filename, bool print, ShaderDefineContainer& container );
	GFX_API void SetUniformFloat ( const std::string& name, const float value );
	GFX_API void SetUniformInt ( const std::string& name, const int value );
	GFX_API void SetUniformUInt ( const std::string& name, const unsigned int value );
	GFX_API void SetUniformBool ( const std::string& name, const bool value );
	GFX_API void SetUniformVec3 ( const std::string& name, const glm::vec3& value );
	GFX_API void SetUniformMat4 ( const std::string& name, const glm::mat4x4& value );
	GFX_API void SetUniformVec2 ( const std::string& name, const glm::vec2& value );
	GFX_API void SetUniformVec4 ( const std::string& name, const glm::vec4& value );
	GFX_API void SetUniformTexture(const std::string& name, Texture& tex);
	GFX_API void SetUniformTextureHandle(const std::string& name, GLuint tex, int index);
	GFX_API GLint FetchUniform ( const std::string& name );
	GFX_API bool Recompile();

	void SaveProgramBinary();
	void LoadProgramBinary(const std::string& filename);
	bool LoadProgramBinaryHeader(const std::string& filename, ShaderProgramBinary& outHeader);
  private:
	GLuint                  m_Handle;
	std::vector<Shader*>		m_Shaders;
	bool                    m_LoadedShaders;
	std::map<std::string, GLint>    m_UniformMap;
	int                     m_TextureCount;
	bool                    m_Validated = false;
	std::string                 m_Filename;
	ShaderDefineContainer   m_Defines;

	void CreateShader ( std::ifstream* FileStream, GLenum shaderType, const std::string& filename, bool print );
	void CreateShader ( std::ifstream* FileStream, GLenum shaderType, const std::string& filename, bool print, ShaderDefineContainer& container );

	std::string LoadText ( const char* filename );
};
}

