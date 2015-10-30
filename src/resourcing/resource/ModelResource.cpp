#include "ModelResource.h"

ModelResource::ModelResource() {
}

ModelResource::~ModelResource() {

}

void ModelResource::CalcSize() {
	size_t size = 0;
	size += sizeof(Vertex) *	Vertices.size();
	size += sizeof(GLuint) *	Indices.size();
	size += sizeof(Mesh) *		Meshes.size();
	size += sizeof(Material) *  Materials.size();
	size += sizeof(unsigned int);
	this->SetSize(size);
}