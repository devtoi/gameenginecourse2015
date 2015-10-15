#include "RenderQueue.h"
#include "BufferManager.h"
#include "ShaderBank.h"
gfx::RenderQueue::RenderQueue() {
	m_ModelQueue.reserve(50);
	m_TerrainQueue.reserve(20);
	m_ShaderInputBuffer.reserve(MAX_OBJECTS);
	m_TransparentShaderInputBuffer.reserve(MAX_OBJECTS);
}

gfx::RenderQueue::~RenderQueue() {

}

void gfx::RenderQueue::CreateBuffer() {
	g_BufferManager.CreateBuffer("ShaderInputs", GL_SHADER_STORAGE_BUFFER, SIZE_OF_OBJECT * MAX_OBJECTS, GL_DYNAMIC_DRAW);
	g_BufferManager.CreateBuffer("TransparentShaderInputs", GL_SHADER_STORAGE_BUFFER, SIZE_OF_OBJECT * MAX_OBJECTS, GL_DYNAMIC_DRAW);
}

void gfx::RenderQueue::UpdateBuffer() {
	g_BufferManager.UpdateBuffer("ShaderInputs", 0, m_ShaderInputBuffer.data(), (int)(m_ShaderInputBuffer.size() * sizeof(ShaderInput)));
	g_BufferManager.UpdateBuffer("TransparentShaderInputs", 0, m_TransparentShaderInputBuffer.data(), (int)(m_TransparentShaderInputBuffer.size() * sizeof(ShaderInput)));
}

void gfx::RenderQueue::Enqueue(ModelHandle model, const std::vector<ShaderInput>& inputs) {
	ModelObject mo;
	mo.Model = model;
	mo.InstanceCount = (int)inputs.size();
	m_ModelQueue.push_back(mo);
	m_ShaderInputBuffer.insert(m_ShaderInputBuffer.end(), inputs.begin(), inputs.end());
}

void gfx::RenderQueue::Enqueue(ModelHandle model, const ShaderInput& input) {
	ModelObject mo;
	mo.Model = model;
	mo.InstanceCount = 1;
	m_ModelQueue.push_back(mo);
	m_ShaderInputBuffer.insert(m_ShaderInputBuffer.end(), input);
}

void gfx::RenderQueue::Enqueue(ModelHandle model, const std::vector<ShaderInput>& inputs, float transparency) {
	TransparentModelObject mo;
	mo.Model = model;
	mo.InstanceCount = (int)inputs.size();
	mo.Transparency = transparency;
	m_TransparentModelQueue.push_back(mo);
	m_TransparentShaderInputBuffer.insert(m_TransparentShaderInputBuffer.end(), inputs.begin(), inputs.end());
}

void gfx::RenderQueue::Enqueue(ModelHandle model, const ShaderInput& input, float transparency) {
	TransparentModelObject mo;
	mo.Model = model;
	mo.InstanceCount = 1;
	mo.Transparency = transparency;
	m_TransparentModelQueue.push_back(mo);
	m_TransparentShaderInputBuffer.insert(m_TransparentShaderInputBuffer.end(), input);
}
void gfx::RenderQueue::Enqueue(const gfx::TerrainDeformation& td){
	m_TerrainDeformations.push_back(td);
}

void gfx::RenderQueue::Enqueue(const TerrainObject& to) {
	m_TerrainQueue.push_back(to);
}

void gfx::RenderQueue::Enqueue(const DeferedDecal& dd) {
	m_DeferedDecals.push_back(dd);
}

void gfx::RenderQueue::Clear() {
	m_ModelQueue.clear();
	m_TerrainQueue.clear();
	m_ShaderInputBuffer.clear();
	m_TransparentShaderInputBuffer.clear();
	m_Views.clear();
	m_TerrainDeformations.clear();
	m_DeferedDecals.clear();
	m_TransparentModelQueue.clear();
}
