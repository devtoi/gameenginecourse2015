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
	m_Models.push_back(model);
	m_Updated = true;
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
	if ((int)m_Models.size() <= 0 || !m_Updated) {
		return;
	}
	// Vertex buffer
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<GLuint> modelIndices;
	GLuint indexCounter = 0;
	GLuint vertexCounter = 0;
	for (auto& model : m_Models) {
		//set offset
		model->IndexOffset = indexCounter;
		//update indices
		for (auto& i : model->Indices) {
			modelIndices.push_back(i + vertexCounter);
		}
		//insert vertices and indices
		vertices.insert(vertices.end(), model->Vertices.begin(), model->Vertices.end());
		indices.insert(indices.end(), modelIndices.begin(), modelIndices.end());
		indexCounter += modelIndices.size();
		vertexCounter += model->Vertices.size();
		modelIndices.clear();
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size( ), indices.data() ,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLsync fenceId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	GLenum result;
	while (true)
	{
		result = glClientWaitSync(fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(5000000000)); //5 Second timeout 
		if (result != GL_TIMEOUT_EXPIRED) break; //we ignore timeouts and wait until all OpenGL commands are processed! 
	}
	m_Updated = false;
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
	//auto model = m_Models.find(id);
	//if (model != m_Models.end()) {
	//	return model->second;
	//}
	return nullptr;
}