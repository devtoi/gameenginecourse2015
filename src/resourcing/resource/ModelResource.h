#pragma once
#include "../Resource.h"
#include "Material.h"
#include <glm/glm.hpp>
#include <vector>
struct Vertex {
	glm::vec4 Position;
	glm::vec4 Normal;
	glm::vec4 TexCoord;
	glm::vec4 Tangent;
};
struct Mesh {
	unsigned int VertexOffset;
	unsigned int IndexOffset;
	unsigned int Material;
	unsigned int IndexCount;
	unsigned int VertexCount;
};
class ModelResource : public Resource {
public:
	ModelResource();
	~ModelResource();
	void CalcSize();

	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<Mesh> Meshes;
	std::vector<Material*> Materials;
	unsigned int IndexOffset = 0;
};