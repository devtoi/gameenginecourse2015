#pragma once
#include <GL/glew.h>
#include <string>
#include <map>
#include <vector>
#include "ShaderBank.h"
namespace gfx {
#define g_BufferManager BufferManager::GetInstance()
class BufferManager {
  public:
	~BufferManager();
	static BufferManager& GetInstance();
	GLuint CreateBuffer(const std::string& name, GLenum type, int size, GLenum usage);
	GLuint CreateBufferWithData(const std::string& name, GLenum type, int size, GLenum usage, void* data);
	void BindBufferToProgram(const std::string& name, ShaderProgram* program, GLuint bindingpoint);
	void BindBufferWithRange(const std::string& name, ShaderProgram* program, GLuint bindingpoint, int size);
	void ResizeBuffer(const std::string& name, int newsize);
	void UpdateBuffer(const std::string& name, int offset, void* data, int size);
	void* MapBuffer(const std::string& name, GLenum usage);
	void UnmapBuffer();
	bool IsProgramBound(ShaderProgramHandle program, GLuint bindingpoint);
	bool IsBufferBound(GLuint buffer, GLuint bindingpoint);
	void ClearBuffers();
  private:
	BufferManager();
	std::map<std::string, GLuint>							m_Buffers; //Name to buffers
	std::map<GLuint, GLenum>								m_BufferTypes; //Buffers to types
	std::map<GLuint, GLuint>								m_Bindings; //Buffers bound to bindingpoints
	std::map<ShaderProgramHandle, std::vector<GLuint>>		m_ProgramBindings; //ShaderPrograms to bindingpoints
	GLint													m_CurrentMappedBuffer;
};
}
