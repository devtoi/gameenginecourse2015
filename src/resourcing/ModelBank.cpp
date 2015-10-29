#include "ModelBank.h"
#include <GL/glew.h>

ModelBank::ModelBank() {
	m_IndexBuffer = 0;
}

ModelBank::~ModelBank() {
	this->Clear();
}

ModelBank& ModelBank::GetInstance() {
	static ModelBank m_Bank;
	return m_Bank;
}

void ModelBank::AddModel(ModelResource* model, const ResourceIdentifier id) {
	m_Models[id] = model;
}

void ModelBank::Init() {
	glGenBuffers(1, &m_VertexBuffer);
	glGenBuffers(1, &m_IndexBuffer);
}

void ModelBank::DeleteModel(const ResourceIdentifier id) {
	
}

void ModelBank::Clear() {
	//TODO:Redo
}

void ModelBank::BuildBuffers() {
	if ((int)m_Models.size() <= 0) {
		return;
	}
	// Vertex buffer
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	GLuint indexCounter = 0;
	GLuint vertexCounter = 0;
	for (auto& model : m_Models) {
		//set offset
		model.second->IndexOffset = indexCounter;
		//update indices to
		for (auto& i : model.second->Indices) {
			i += vertexCounter;
		}
		//insert vertices and indices
		vertices.insert(vertices.end(), model.second->Vertices.begin(), model.second->Vertices.end());
		indices.insert(indices.end(), model.second->Indices.begin(), model.second->Indices.end());
		indexCounter += model.second->Indices.size();
		vertexCounter += model.second->Vertices.size();
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size( ), indices.data(), GL_STATIC_DRAW );
}

bool ModelBank::ApplyBuffers() {
	if (m_VertexBuffer != 0 && m_IndexBuffer != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		return true;
	}
	return false;
}

ModelResource* ModelBank::GetModel(const ResourceIdentifier id) {
	auto model = m_Models.find(id);
	if (model != m_Models.end()) {
		return model->second;
	}
	return nullptr;
}