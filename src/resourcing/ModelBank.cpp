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
		indexCounter += indices.size();
		vertexCounter += vertices.size();
	}
	glGenVertexArrays(1, &m_VertexArrayObject);
	glGenBuffers(1, &m_VertexBuffer);

	glBindVertexArray(m_VertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (unsigned char*)0 + (4 * sizeof(float)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (unsigned char*)0 + (8 * sizeof(float)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (unsigned char*)0 + (12 * sizeof(float)));
	
	glGenBuffers( 1, &m_IndexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size( ), indices.data(), GL_STATIC_DRAW );
}

void ModelBank::ApplyBuffers() {
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBindVertexArray(m_VertexArrayObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
}
