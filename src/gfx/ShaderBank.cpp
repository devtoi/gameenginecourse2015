#include "ShaderBank.h"
gfx::ShaderBank::ShaderBank() {
	m_Counter = 0;
}

gfx::ShaderBank::~ShaderBank() {
	for ( std::vector<ShaderProgram *>::iterator it = m_Programs.begin(); it != m_Programs.end(); it++ ) {
		delete *it ;
	}
	m_Programs.clear();
	m_HandleToPrograms.clear();
}

gfx::ShaderBank& gfx::ShaderBank::GetInstance() {
	static ShaderBank m_Bank;
	return m_Bank;
}

gfx::ShaderProgramHandle gfx::ShaderBank::LoadShaderProgram ( const std::string& filename ) {
	//find if its already in bank
	for(auto& it : m_HandleToPrograms) {
		if(it.second->GetFilename() == filename) {
			//Logger::Log( "Program filename already exist in the bank, use defines or a bitflag to vary shader compilation", "ShaderBank", LogSeverity::WARNING_MSG );
			return it.first;
		}
	}
	ShaderProgram* program = new ShaderProgram( );
#ifdef SHADER_BINARY_PROGRAM

	ShaderProgramBinary header;
	if(program->LoadProgramBinaryHeader(filename, header)) {
		if(strcmp(header.Version, (char*)glGetString(GL_VERSION)) == 0) {
			program->LoadProgramBinary(filename);
			ShaderProgramHandle handle = m_Counter++;
			m_Programs.push_back(program);
			m_HandleToPrograms.emplace( handle, program );
			return handle;
		}
	}
	if(program->LoadCompleteShaderProgramFromFile(filename, true)) {
		program->SaveProgramBinary();
	}
#else
	program->LoadCompleteShaderProgramFromFile(filename, true);
#endif
	ShaderProgramHandle handle = m_Counter++;
	m_Programs.push_back(program);
	m_HandleToPrograms.emplace( handle, program );
	return handle;
}
gfx::ShaderProgramHandle gfx::ShaderBank::LoadShaderProgramWithDefines ( const std::string& filename, ShaderDefineContainer& defines ) {
	ShaderProgram* program = new ShaderProgram ();
#ifdef SHADER_BINARY_PROGRAM

	ShaderProgramBinary header;
	if(program->LoadProgramBinaryHeader(filename, header)) {
		if(strcmp(header.Version, (char*)glGetString(GL_VERSION)) == 0) {
			program->LoadProgramBinary(filename);
			ShaderProgramHandle handle = m_Counter++;
			m_Programs.push_back(program);
			m_HandleToPrograms.emplace( handle, program );
			return handle;
		}
	}
	if(program->LoadCompleteShaderProgramFromFile(filename, true, defines)) {
		program->SaveProgramBinary();
	}
#else
	//program->LoadCompleteShaderProgramFromFile(filename, true, defines);
#endif
	ShaderProgramHandle handle = m_Counter++;
	m_Programs.push_back(program);
	m_HandleToPrograms.emplace( handle, program );
	return handle;
}

gfx::ShaderProgramHandle gfx::ShaderBank::LoadShaderProgramFromShaders ( const std::vector< Shader* >& shaders ) {
	ShaderProgram* program = new ShaderProgram();
	program->Init(shaders, true);
	ShaderProgramHandle handle = m_Counter++;
	m_Programs.push_back(program);
	m_HandleToPrograms.emplace(handle, program);
	return handle;
}

gfx::ShaderProgram* gfx::ShaderBank::GetProgramFromHandle (gfx::ShaderProgramHandle handle ) {
	std::map<ShaderProgramHandle, ShaderProgram*>::const_iterator it = m_HandleToPrograms.find(handle);
	if(it == m_HandleToPrograms.end()) {
		return nullptr;
	} else {
		return m_HandleToPrograms[handle];
	}
}

void gfx::ShaderBank::RecompileAllShaders() {
	for(auto& it : m_HandleToPrograms) {
		it.second->Recompile();
	}
	//printf("\n");
	//Logger::Log( "Recompiled all shaders", "ShaderBank", LogSeverity::DEBUG_MSG );
}

void gfx::ShaderBank::Clear() {
	//TODOHJ: Clear
}