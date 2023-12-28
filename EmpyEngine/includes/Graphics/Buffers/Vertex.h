#pragma once
#include "Common/Core.h"

namespace Empy
{
	// quad vertex 
	struct QuadVertex 
	{
		float Data[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	};

	// flat vertex
	struct FlatVertex 
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec4 Color = glm::vec4(0.0f);
	};   

	// shading vertex
	struct ShadedVertex 
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Normal = glm::vec3(0.0f);
		glm::vec2 UVs = glm::vec2(0.0f);
	};

	// mesh data
	template <typename Vertex> 
	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
	};
}