#pragma once
#include <GL/glew.h>
#include "Vertex.h"
#include <vector>
namespace gfx {
enum VertexType {
	POS,
	POS_NORMAL,
	POS_TEX,
	POS_NORMAL_TEX,
	POS_NORMAL_TEX_TEXEL,
	POS_NORMAL_TEX_TANGENT
};
struct InputLayout {
	struct Attribute {
		GLenum Type;
		int Count;
		int Size;
		int Offset;
	};
	std::vector<Attribute> Attributes;
};
class VertexBuffer {
  private:
	GLuint m_Handle;
	GLuint m_VertexArrayId;
  public:
	VertexBuffer ( void );
	VertexBuffer ( VertexType VertexType, void* Data, unsigned int Size );
	~VertexBuffer ( void );
	bool Init ( VertexType VertexType, void* Data, unsigned int Size);
	bool Init ( InputLayout Layout, void* Data, unsigned int Size);
	void Apply();
	void* Map ( GLenum Access );
	void UnMap();
	void Release();
};
}
