#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
namespace gfx {
class ShaderDefineContainer {
  public:
	ShaderDefineContainer();
	~ShaderDefineContainer();

	void AddDefine ( const std::string& defineName );
	void AddDefineToShaderStage ( const std::string& defineName, GLenum shaderStage );
	std::string GetAllDefineLines();
	std::string GetDefinesShaderStage ( GLenum shaderStage );
	bool IsEmpty();
  private:
	std::vector<std::string> m_VertexDefines;
	std::vector<std::string> m_GeometryDefines;
	std::vector<std::string> m_FragmentDefines;
	std::vector<std::string> m_ControlDefines;
	std::vector<std::string> m_EvaluationDefines;
	std::vector<std::string> m_ComputeDefines;
	std::vector<std::string> m_GlobalDefines;
	bool m_Empty;
};
}