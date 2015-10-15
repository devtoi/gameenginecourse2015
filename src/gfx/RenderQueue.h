#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "GFXLibraryDefine.h"
namespace gfx {
typedef int ModelHandle;
typedef unsigned short TerrainHandle;

struct ModelObject {
	ModelHandle Model;
	unsigned int InstanceCount;
};

struct TransparentModelObject {
	ModelHandle Model;
	unsigned int InstanceCount;
	float Transparency;
};

struct ShaderInput {
	glm::mat4 World;
	glm::vec4 Color;
};

struct CameraData {
	glm::mat4 View;
	glm::mat4 Proj;
	glm::mat4 ProjView;
	glm::vec3 Position;

	glm::vec3 Forward;
	glm::vec3 Right;
	glm::vec3 Up;
	float Fov;
	float Near;
	float Far;
};

struct Viewport {
	float x;
	float y;
	float width;
	float height;
};

struct View {
	CameraData camera;
	Viewport viewport;
};

struct DeferedDecal {
	glm::mat4 World;
	glm::vec4 Tint;
	unsigned int Texture; //loaded with materialbank
};
struct TerrainObject {
	TerrainHandle	Terrain; //Handle for the terrain containing the textures
	glm::mat4		Transform; //Tansformation of terrain. Dont include scale it will be added later in the pipeline
	float			HeightScale; //Scale in Y
	float			Size; //Size in X and Z
	glm::vec3		Color; //Color of terrain
};

struct TerrainDeformation {
	TerrainHandle	Terrain;
	glm::vec2		Position;
	glm::vec2		Size;
	float			Strength;
	bool			Up;
};

class RenderQueue {
  public:
	RenderQueue();
	~RenderQueue();
	GFX_API void Enqueue(ModelHandle model, const std::vector<ShaderInput>& inputs);
	GFX_API void Enqueue(ModelHandle model, const ShaderInput& input);
	GFX_API void Enqueue(ModelHandle model, const std::vector<ShaderInput>& inputs, float transparency);
	GFX_API void Enqueue(ModelHandle model, const ShaderInput& input, float transparency);
	GFX_API void Enqueue(const TerrainDeformation& td);
	GFX_API void Enqueue(const TerrainObject& to);
	GFX_API void Enqueue(const DeferedDecal& dd);
	GFX_API void Clear();
	void CreateBuffer();
	void UpdateBuffer();

	void AddView(const View& v) {
		m_Views.push_back(v);
	}
	const std::vector<ModelObject>& GetModelQueue() const {
		return m_ModelQueue;
	}
	const std::vector<TransparentModelObject>& GetTranparentModelQueue() const {
		return m_TransparentModelQueue;
	}
	const std::vector<TerrainObject>& GetTerrainQueue() const {
		return m_TerrainQueue;
	}
	const std::vector<View>& GetViews() const {
		return m_Views;
	}
	const std::vector<TerrainDeformation>& GetDeformations() const {
		return m_TerrainDeformations;
	}

	const std::vector<DeferedDecal>& GetDeferedDecals() const {
		return m_DeferedDecals;
	}

  private:
	std::vector<ModelObject>			m_ModelQueue;
	std::vector<TransparentModelObject>	m_TransparentModelQueue;
	std::vector<TerrainObject>			m_TerrainQueue;
	std::vector<View>					m_Views;
	std::vector<ShaderInput>			m_ShaderInputBuffer;
	std::vector<ShaderInput>			m_TransparentShaderInputBuffer;
	std::vector<TerrainDeformation>		m_TerrainDeformations;
	std::vector<DeferedDecal>			m_DeferedDecals;
	const int						MAX_OBJECTS = 1000; //change this to increase the number of items to render //also need to update shaders
	const int						SIZE_OF_OBJECT = sizeof(ShaderInput);
};
}