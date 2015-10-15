#include "Shader.h"
#include <string>
#include <fstream>
gfx::Shader::Shader() {
}

gfx::Shader::~Shader() {
	glDeleteShader ( m_Handle );
}

bool gfx::Shader::CreateFromFile( const std::string& Filename, GLenum ShaderType, bool print ) {
	m_Path = Filename;
	m_Type = ShaderType;
	std::ifstream fin;
	fin.open ( Filename.c_str() );
	if ( !fin.is_open() ) {
		if ( print ) {
			printf("Error opening shader file %s\n",Filename.c_str());
			//Logger::Log("Error opening shader file " + Filename, "Shader", LogSeverity::ERROR_MSG);

		}
		return false;
	}
	std::string shadertext;
	size_t found;
	std::string line;
	while ( !fin.eof() ) {
		std::getline( fin, line );
		found = line.find ( "#include " );
		if ( found != std::string::npos ) {
			int i = static_cast<int> ( found ) + 9; //found + "#include "
			std::string s;
			while ( i < line.length() ) {
				s += line[i];
				i++;
			}
			std::string str = GetDir ( std::string ( Filename ) ) + s;
			shadertext += LoadText ( str.c_str() );
			shadertext += "\n";
		} else {
			shadertext += line;
			shadertext += '\n';
		}
	}
	fin.close();

	//create shader object
	m_Handle = glCreateShader ( ShaderType );
	if ( m_Handle == 0 ) {
		if ( print ) {
			//Logger::Log( "Error creating shader", "Shader", LogSeverity::ERROR_MSG );
		}
	}
	const char* text = shadertext.c_str();
	glShaderSource ( m_Handle, 1, &text, nullptr );

	glCompileShader ( m_Handle );
	int result = 0;
	glGetShaderiv ( m_Handle, GL_COMPILE_STATUS, &result );
	if ( result == GL_FALSE ) {
		if ( print ) {
			//Logger::Log( "ERROR, compiling shader " + Filename, "Shader", LogSeverity::ERROR_MSG );
		}
		int length = 0;
		glGetShaderiv ( m_Handle, GL_INFO_LOG_LENGTH, &length );
		if ( length > 0 ) {
			// create a log of error messages
			char* errorLog = new char[length];
			int written = 0;

			glGetShaderInfoLog ( m_Handle, length, &written, errorLog );
			if ( print ) {
				//Logger::Log( "Error log: " + std::string( errorLog ), "Shader", LogSeverity::ERROR_MSG );
			}
			delete [] errorLog;
		}
		return false;
	} else {
		if ( print ) {
			//Logger::Log( "Compiled Shader " + Filename, "Shader", LogSeverity::DEBUG_MSG );
		}
	}
	return true;
}

bool gfx::Shader::CreateFromString( std::string ShaderCode, GLenum ShaderType, const std::string& filename, bool print ) {
	//create shader object
	m_Handle = glCreateShader ( ShaderType );
	if ( m_Handle == 0 ) {
		if ( print ) {
			//Logger::Log( "Error creating shader", "Shader", LogSeverity::ERROR_MSG );
		}
	}
	const char* text = ShaderCode.c_str();
	glShaderSource ( m_Handle, 1, &text, nullptr );

	glCompileShader ( m_Handle );
	int result = 0;
	glGetShaderiv ( m_Handle, GL_COMPILE_STATUS, &result );
	if ( result == GL_FALSE ) {
		if ( print ) {
			//Logger::Log("Error compiling shader", "Shader", LogSeverity::ERROR_MSG);
		}
		int length = 0;
		glGetShaderiv ( m_Handle, GL_INFO_LOG_LENGTH, &length );
		if ( length > 0 ) {
			// create a log of error messages
			char* errorLog = new char[length];
			int written = 0;
			glGetShaderInfoLog ( m_Handle, length, &written, errorLog );
			if ( print ) {
				//printf( "Error log:\n %s\n", errorLog );
				//Logger::Log("Error log :" + std::string(errorLog), "Shader", LogSeverity::ERROR_MSG);
				//dump shader to file
				std::ofstream fout("../../../shader/ErrorShader.txt");
				fout << ShaderCode;
				fout.close();

			}
			delete[] errorLog;
		}
		return false;
	} else {
		if ( print ) {
			std::string type;
			if( ShaderType == GL_VERTEX_SHADER )
				type = "vertex";
			else if( ShaderType == GL_GEOMETRY_SHADER )
				type = "geometry";
			else if( ShaderType == GL_TESS_CONTROL_SHADER )
				type = "control";
			else if( ShaderType == GL_TESS_EVALUATION_SHADER )
				type = "evaluation";
			else if( ShaderType == GL_FRAGMENT_SHADER )
				type = "fragment";
			else if( ShaderType == GL_COMPUTE_SHADER )
				type = "compute";

			//Logger::Log( "Compiled " + type + " Shader " + filename , "Shader", LogSeverity::DEBUG_MSG );
		}
	}
	return true;
}

GLuint gfx::Shader::GetHandle() {
	return m_Handle;
}

std::string gfx::Shader::GetDir( std::string filename ) {
	bool found = false;
	for ( int i = static_cast<int> ( filename.size() ); i > 0; i-- ) {
		if ( filename.c_str() [i] == '/' ) {
			found = true;
		}
		if ( !found ) {
			filename.erase ( i );
		}

	}
	return filename;
}

std::string gfx::Shader::LoadText( const std::string& filename ) {
	std::ifstream fin;
	fin.open ( filename.c_str() );
	if ( !fin.is_open() ) {
		return "";
	}
	std::string text;
	std::string line;
	while ( !fin.eof() ) {
		getline ( fin, line );
		text += line;
	}
	return text;
}

void gfx::Shader::Clear() {
	//when a shader has been linked to a program  there is no longer any need to save the shader(unless you want to reuse the shader)
	//so we clear shaders after we are done with them to free up memory.
	glDeleteShader ( m_Handle );
}

void gfx::Shader::Recompile() {
	Clear();
	CreateFromFile ( m_Path, m_Type, false );
}
