#include "ShaderDefineContainer.h"
#include <sstream>
#include <vector>
#include <sstream>
gfx::ShaderDefineContainer::ShaderDefineContainer() {
	m_Empty = true;
}

gfx::ShaderDefineContainer::~ShaderDefineContainer() {

}
//Defines should be name and value only.
//#define will be added automaticly
void gfx::ShaderDefineContainer::AddDefine ( const std::string& defineName ) {
	m_GlobalDefines.push_back ( defineName );
	m_Empty = false;
}

void gfx::ShaderDefineContainer::AddDefineToShaderStage ( const std::string& defineName, GLenum shaderStage ) {
	switch ( shaderStage ) {
	case GL_VERTEX_SHADER: {
		m_VertexDefines.push_back ( defineName );
	}
	break;
	case GL_GEOMETRY_SHADER: {
		m_GeometryDefines.push_back ( defineName );
	}
	break;
	case GL_TESS_CONTROL_SHADER: {
		m_ControlDefines.push_back ( defineName );
	}
	break;
	case GL_TESS_EVALUATION_SHADER: {
		m_EvaluationDefines.push_back ( defineName );
	}
	break;
	case GL_FRAGMENT_SHADER: {
		m_FragmentDefines.push_back ( defineName );
	}
	break;
	case GL_COMPUTE_SHADER: {
		m_ComputeDefines.push_back ( defineName );
	}
	break;
	}
	m_Empty = false;
}

std::string gfx::ShaderDefineContainer::GetDefinesShaderStage ( GLenum shaderStage ) {
	std::stringstream ss;
	switch ( shaderStage ) {
	case GL_VERTEX_SHADER: {
		for ( auto& it : m_VertexDefines ) {
			ss << "\n #define " << it << " \n";
		}
	}
	break;
	case GL_GEOMETRY_SHADER: {
		for ( auto& it : m_GeometryDefines ) {
			ss << "\n #define " << it << " \n";
		}
	}
	break;
	case GL_TESS_CONTROL_SHADER: {
		for ( auto& it : m_ControlDefines ) {
			ss << "\n #define " << it << " \n";
		}
	}
	break;
	case GL_TESS_EVALUATION_SHADER: {
		for ( auto& it : m_EvaluationDefines ) {
			ss << "\n #define " << it << " \n";
		}
	}
	break;
	case GL_FRAGMENT_SHADER: {
		for ( auto& it : m_FragmentDefines ) {
			ss << "\n #define " << it << " \n";
		}
	}
	break;
	case GL_COMPUTE_SHADER: {
		for ( auto& it : m_ComputeDefines ) {
			ss << "\n #define " << it << " \n";
		}
	}
	break;
	}
	return std::string(ss.str().c_str());
}

std::string gfx::ShaderDefineContainer::GetAllDefineLines() {
	std::stringstream ss;
	for ( auto& it : m_GlobalDefines ) {
		ss << "\n #define " << it << " \n";
	}
	return std::string(ss.str().c_str());
}

bool gfx::ShaderDefineContainer::IsEmpty() {
	return m_Empty;
}
