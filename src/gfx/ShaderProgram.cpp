#include "ShaderProgram.h"
#include <sstream>
#include <fstream>
#include <cstring>
#include <utility/Logger.h>
#include <utility/FileUtility.h>

using gfx::Shader;

gfx::ShaderProgram::ShaderProgram ( void ) {
	m_LoadedShaders = false;
	m_TextureCount = 0;
}

gfx::ShaderProgram::ShaderProgram ( std::vector<Shader*> Shaders ) {
	this->m_Shaders = Shaders;
	Init ( m_Shaders, true );
}

gfx::ShaderProgram::~ShaderProgram ( void ) {
	if ( m_LoadedShaders ) {
		for ( std::vector<Shader*>::iterator it = m_Shaders.begin(); it != m_Shaders.end(); ++it ) {
			delete *it ;
		}
		m_Shaders.clear();
	}

	glDeleteProgram ( m_Handle );
}

bool gfx::ShaderProgram::Init ( std::vector<Shader*> Shaders, bool print ) {
	if ( m_Shaders.size() > 0 ) {
		m_Shaders.clear();
	}
	//save shaders
	for ( auto& it : Shaders ) {
		m_Shaders.push_back ( it );
	}
	m_Handle = glCreateProgram();
	if ( m_Handle == 0 ) {
		throw "Failed to create shader program";
		return false;
	}
	//attach shaders to program
	for ( int i = 0; i < static_cast<int> ( Shaders.size() ); i++ ) {
		glAttachShader ( m_Handle, Shaders[i]->GetHandle() );
	}
	//link shaders
	glLinkProgram ( m_Handle );
	//error checking
	GLint status;
	glGetProgramiv ( m_Handle, GL_LINK_STATUS, &status );
	if ( status == GL_FALSE ) {
		char log[1024];
		int len = 0;
		glGetProgramInfoLog ( m_Handle, sizeof ( log ), &len, log );
		//printf( "Error linking shaders\n Error log:\n %s \n",log );
		Logger::Log( "Errorlog: " + std::string( log ), "ShaderProgram", LogSeverity::ERROR_MSG );
		return false;
	}
	if ( print ) {
		Logger::Log("Linked ShaderProgram", "ShaderProgram", LogSeverity::INFO_MSG);
	}
	// Not using "return Validate();" because stuff might get added after
	if ( !Validate() ) {
		return false;
	}
	return true;
}

GLuint gfx::ShaderProgram::GetHandle() {
	return m_Handle;
}

void gfx::ShaderProgram::Apply() {
	// Lazy validation because of AMD driver behaviour.
	if ( !m_Validated ) {
		if ( !Validate() ) {
			GLint status;
			glGetProgramiv ( m_Handle, GL_VALIDATE_STATUS, &status );
			if ( status == GL_FALSE ) {
				char log[1024];
				int len = 0;
				glGetProgramInfoLog ( m_Handle, sizeof ( log ), &len, log );

				if (len > 0) {
					Logger::Log("Validation of shader program: \"" + m_Filename + "\" failed: " + std::string(log, len - 1), "ShaderProgram", LogSeverity::ERROR_MSG);
				}
				//assert ( false );
			}
		}
	}

	m_TextureCount = 0;
	glUseProgram ( m_Handle );
}

bool gfx::ShaderProgram::Validate() {
	glValidateProgram ( m_Handle );
	GLint status;
	glGetProgramiv ( m_Handle, GL_VALIDATE_STATUS, &status );
	m_Validated = ( status == GL_TRUE );
	return m_Validated;
}

GLuint gfx::ShaderProgram::GetUniformLocationByName ( const std::string& name ) {
	return glGetUniformLocation ( m_Handle, name.c_str() );
}
//Create shaderprogram
bool gfx::ShaderProgram::LoadCompleteShaderProgramFromFile ( const std::string& filename, bool print ) {
	//this function is used to load a set of shaders from a single file
	//seperate each shader with a #new_shader ******* and a #end_shader
	//where **** can be a vertex, geometry, control, evaluation or fragment
	//extra code can be loaded into the current shader by the #include filename macro
	m_Filename = std::string ( filename.c_str() );
	m_LoadedShaders = true;
	std::ifstream fin;
	fin.open ( filename.c_str() );
	if ( !fin.is_open() ) {
		if ( print ) {
			Logger::Log( "Could not open file " + filename, "ShaderProgram", LogSeverity::ERROR_MSG );
			//printf("Error opening shader file %s \n",filename.c_str());
			return false;
		}
		return false;
	}
	std::string line;
	size_t found;
	while ( !fin.eof() ) {
		getline ( fin, line );
		if ( (found = line.find( "#new_shader" )) != std::string::npos ) {
			if ( (found = line.find( "vertex" )) != std::string::npos ) {
				CreateShader ( &fin, GL_VERTEX_SHADER, filename, print );
			}
			if ( (found = line.find( "geometry" )) != std::string::npos ) {
				CreateShader ( &fin, GL_GEOMETRY_SHADER, filename, print );
			}
			if ( (found = line.find( "control" )) != std::string::npos ) {
				CreateShader ( &fin, GL_TESS_CONTROL_SHADER, filename, print );
			}
			if ( (found = line.find( "evaluation" )) != std::string::npos ) {
				CreateShader ( &fin, GL_TESS_EVALUATION_SHADER, filename, print );
			}
			if ( (found = line.find( "fragment" )) != std::string::npos ) {
				CreateShader ( &fin, GL_FRAGMENT_SHADER, filename, print );
			}
			if ( (found = line.find( "compute" )) != std::string::npos ) {
				CreateShader ( &fin, GL_COMPUTE_SHADER, filename, print );
			}
		}
	}

	m_Handle = glCreateProgram();
	if ( m_Handle == 0 ) {
		if ( print ) {
			Logger::Log( "Could not create shader program", "ShaderProgram", LogSeverity::ERROR_MSG );
			return false;
		}
		return false;
	}
	//attach shaders to program
	for ( int i = 0; i < static_cast<int> ( m_Shaders.size() ); i++ ) {
		glAttachShader ( m_Handle, m_Shaders[i]->GetHandle() );
	}
	//link shaders
	glLinkProgram ( m_Handle );
	//error checking
	GLint status;
	glGetProgramiv ( m_Handle, GL_LINK_STATUS, &status );
	if ( status == GL_FALSE ) {
		char log[1024];
		int len = 0;
		glGetProgramInfoLog ( m_Handle, sizeof ( log ), &len, log );
		//printf( "Error linking shaders\n Error log:\n %s \n", log );
		Logger::Log("Errorlog: " + std::string(log), "ShaderProgram", LogSeverity::ERROR_MSG);
		return false;
	}
	if ( print ) {
		//printf("Loaded shaderprogram %s \n", filename.c_str());
		Logger::Log("Linked ShaderProgram", "ShaderProgram", LogSeverity::INFO_MSG);
	}
	Validate();


	return true;
}

//Creates a shader
void gfx::ShaderProgram::CreateShader(std::ifstream* FileStream, GLenum shaderType, const std::string& filename, bool print) {
	std::string line;
	size_t found;

	std::string shaderText;
	bool end = false;
	while (!end) {
		getline(*FileStream, line);
		//search for the end of the shader
		found = line.find("#include ");
		if (found != std::string::npos) {
			int i = static_cast<int> (found) + 9; //found + "#include "
			std::string s;
			while (i < static_cast<int> (line.length())) {
				s += line[i];
				i++;
			}
			std::string str = FileUtility::GetDirectoryFromFilePath(std::string(filename)) + "/" + s;
			shaderText += LoadText(str.c_str());
			shaderText += "\n";
		}
		else {
			found = line.find("#end_shader");
			if (found != std::string::npos) {
				//break loop
				end = true;
				break;
			}
			else if (FileStream->eof()) {
				Logger::Log("Could not find end of file " + filename, "ShaderProgram", LogSeverity::ERROR_MSG);
				return;
			}
			else {
				shaderText += line;
				shaderText += '\n';
			}

		}

	}
	if (shaderText.length() > 0) {
		Shader* shader = new Shader();
		shader->CreateFromString(shaderText, shaderType, filename, print);
		m_Shaders.push_back(shader);
		shaderText.clear();
	}
}

std::string gfx::ShaderProgram::LoadText ( const char* filename ) {
	std::ifstream fin;
	fin.open ( filename );
	if ( !fin.is_open() ) {
		return "";
	}
	std::string text;
	std::string line;
	while ( !fin.eof() ) {
		getline ( fin, line );
		text += line;
		text += "\n";
	}
	return text;

}

void gfx::ShaderProgram::SetUniformFloat ( const std::string& name, const float value ) {
	glUniform1f ( FetchUniform ( name ), value );
}

void gfx::ShaderProgram::SetUniformInt( const std::string& name, const int value ) {
	glUniform1i ( FetchUniform ( name ), value );
}

void gfx::ShaderProgram::SetUniformUInt ( const std::string& name, const unsigned int value ) {
	glUniform1ui ( FetchUniform ( name ), value );
}

void gfx::ShaderProgram::SetUniformBool ( const std::string& name, const bool value ) {
	glUniform1i ( FetchUniform ( name ), value );
}

void gfx::ShaderProgram::SetUniformVec3 ( const std::string& name, const glm::vec3& value ) {
	glUniform3f ( FetchUniform ( name ), value.x, value.y, value.z );
}

void gfx::ShaderProgram::SetUniformVec2 ( const std::string& name, const glm::vec2& value ) {
	glUniform2f ( FetchUniform ( name ), value.x, value.y );
}

void gfx::ShaderProgram::SetUniformVec4 ( const std::string& name, const glm::vec4& value ) {
	glUniform4f ( FetchUniform ( name ), value.x, value.y, value.z, value.w );
}

void gfx::ShaderProgram::SetUniformMat4 ( const std::string& name, const glm::mat4x4& value ) {
	glUniformMatrix4fv ( FetchUniform ( name ), 1, GL_FALSE, &value[0][0] );
}

void gfx::ShaderProgram::SetUniformTextureHandle(const std::string& name, GLuint tex, int index) {
	glUniform1i(FetchUniform(name), index);
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, tex);
}

GLint gfx::ShaderProgram::FetchUniform ( const std::string& name ) {
	std::map<std::string, GLint>::const_iterator it = m_UniformMap.find ( name );
	// Uniform wasn't found in cache
	if ( it == m_UniformMap.end() ) {
		GLint loc = GetUniformLocationByName ( name );
		// Don't emplace in cache if uniform was invalid
		if ( loc != -1 ) {
			m_UniformMap.emplace ( name, loc );
		} else {
			//printf("Failed to fetch unifrom : %s\n",name.c_str());
			//assert(false); //makes it annoying to debug shaders
		}
		return loc;// Uniform was found in cache
	} else {
		return it->second;
	}
}

bool gfx::ShaderProgram::Recompile() {
	if ( !m_LoadedShaders ) {
		for ( auto& it : m_Shaders ) {
			it->Recompile();
		}
		glDeleteProgram ( m_Handle );
		m_Validated = false;
		return Init ( m_Shaders, false );;
	} else {
		//clear up old shader
		for ( std::vector<Shader*>::iterator i = m_Shaders.begin(); i != m_Shaders.end(); i++ ) {
			delete  *i;
		}
		m_Shaders.clear();
		glDeleteProgram ( m_Handle );
		//reinit the program
		LoadCompleteShaderProgramFromFile ( m_Filename, false );

		m_Validated = false;
		return true;
	}

}

std::string& gfx::ShaderProgram::GetFilename() {
	return m_Filename;
}

