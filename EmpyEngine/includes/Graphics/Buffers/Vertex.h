#pragma once
#include "Common/Core.h"

namespace Empy
{
	// quad vertex 
	struct QuadVertex 
	{
		float Data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	}; 

	// flat vertex
	struct SkyboxVertex 
	{
		glm::vec3 Position = glm::vec3(0.0f);
	};   

	// shading vertex
	struct ShadedVertex 
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Normal = glm::vec3(0.0f);
		glm::vec2 UVs = glm::vec2(0.0f);

		glm::vec3 Tangent = glm::vec3(0.0f);   
		glm::vec3 Bitangent = glm::vec3(0.0f); 
	};

	// skeletal vertex
	struct SkeletalVertex 
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Normal = glm::vec3(0.0f);
		glm::vec2 UVs = glm::vec2(0.0f);

		// for lighting
		glm::vec3 Tangent = glm::vec3(0.0f);   
		glm::vec3 Bitangent = glm::vec3(0.0f); 

		// for animation
		glm::ivec4 Joints = glm::ivec4(-1);
		glm::vec4 Weights = glm::vec4(0.0f);
	};

	// mesh data
	template <typename Vertex> 
	struct MeshData
	{
		std::vector<uint32_t> Indices;
		std::vector<Vertex> Vertices;
	};
}