#pragma once
#include <glm/glm.hpp>
namespace gfx {
//struct Light
//{
//	vec3 Position;
//	float Range;
//	vec3 Direction;
//	float Intensity;
//	vec4 Color;
//	Light()
//	{
//		Position = vec3(0);
//		Range = 0;
//		Direction = vec3(0);
//		Intensity = 0;
//		Color = vec4(0);
//	}
//};

//struct PointLight
//{
//	vec3 Position;
//	float Range;
//	vec3 Color;
//	float Intensity;
//};

//struct DirectionalLight
//{
//	vec3 Direction;
//	float padd;
//	vec3 Color;
//	float padd2;
//};

struct Light {
	glm::vec4 Color;
	glm::vec3 Position;
	float Intensity;
	glm::vec3 Direction;
	float Length;
	glm::vec3 Padd;
	float Range;
};
}
