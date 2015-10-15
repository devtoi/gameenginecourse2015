#pragma once
#include "Texture.h"
#include <glm/glm.hpp>
#include "VertexBuffer.h"
#include <string>
#include <vector>
namespace gfx {
struct Mesh {
	unsigned int VertexBufferOffset;
	unsigned int IndexBufferOffset;
	int Indices;
	int Size;
	unsigned int Material;
};

struct Model {
	unsigned int IndexHandle; //place in ModelIndexBuffer
	unsigned int VertexHandle; //place in ModelVertexBuffer
	unsigned int MaterialOffset;
	std::string Name;
	int NumVertices;
	int NumIndices;
	bool Loaded;
	glm::vec3 Min;
	glm::vec3 Max;
	std::vector<Mesh> Meshes;
};
}