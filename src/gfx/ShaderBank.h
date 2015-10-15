#pragma once
#include "ShaderDefineContainer.h"
#include "ShaderProgram.h"

//#define SHADER_BINARY_PROGRAM
namespace gfx {
typedef int ShaderProgramHandle;
#define g_ShaderBank ShaderBank::GetInstance()
class ShaderBank {
  public:
	static ShaderBank& GetInstance();
	ShaderProgramHandle LoadShaderProgram(const std::string& filename);
	ShaderProgramHandle LoadShaderProgramWithDefines(const std::string& filename, ShaderDefineContainer& defines);
	ShaderProgramHandle LoadShaderProgramFromShaders(const std::vector<Shader*>& shaders);
	ShaderProgram* GetProgramFromHandle(ShaderProgramHandle handle);
	void RecompileAllShaders();
	void Clear();
  private:
	ShaderBank();
	~ShaderBank();
	std::vector<ShaderProgram*>						m_Programs;
	std::map<ShaderProgramHandle, ShaderProgram*>   m_HandleToPrograms;
	ShaderProgramHandle								m_Counter;
};
}
