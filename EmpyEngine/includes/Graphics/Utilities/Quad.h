#pragma once
#include "../Buffers/Mesh.h"

namespace Empy
{
    using Quad3D = std::unique_ptr<Mesh<ShadedVertex>>;
    using Quad2D = std::unique_ptr<Mesh<QuadVertex>>;

    // creates 2d quad mesh
    EMPY_INLINE Quad2D CreateQuad2D() 
    {
        MeshData<QuadVertex> data;

        data.Vertices = 
        {
            {-1.0f, -1.0f, 0.0f, 0.0f },
            {-1.0f,  1.0f, 0.0f, 1.0f },
            { 1.0f,  1.0f, 1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f, 0.0f }
        };

        data.Indices = 
        {
            0, 1, 2,
            0, 2, 3
        };

        return std::make_unique<Mesh<QuadVertex>>(std::move(data));
    }

    // creates 3d quad mesh
    EMPY_INLINE Quad3D CreateQuad3D() 
    {
        MeshData<ShadedVertex> data;

        // Define vertices for the quad
        ShadedVertex v0, v1, v2, v3;

        v0.Position = glm::vec3(-0.5f, -0.5f, 0.0f); // Bottom-left
        v1.Position = glm::vec3(0.5f, -0.5f, 0.0f);  // Bottom-right
        v2.Position = glm::vec3(0.5f, 0.5f, 0.0f);   // Top-right
        v3.Position = glm::vec3(-0.5f, 0.5f, 0.0f);  // Top-left

        // Define normals (in this case, all normals point in the same direction)
        v0.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v1.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v2.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
        v3.Normal = glm::vec3(0.0f, 0.0f, 1.0f);

        // Define UVs (texture coordinates)
        v0.UVs = glm::vec2(0.0f, 0.0f);
        v1.UVs = glm::vec2(1.0f, 0.0f);
        v2.UVs = glm::vec2(1.0f, 1.0f);
        v3.UVs = glm::vec2(0.0f, 1.0f);

        // Add vertices to the MeshData structure
        data.Vertices.push_back(v0);
        data.Vertices.push_back(v1);
        data.Vertices.push_back(v2);
        data.Vertices.push_back(v3);

        // Define indices to form two triangles (quad)
        data.Indices.push_back(0); // Triangle 1: v0, v1, v2
        data.Indices.push_back(1);
        data.Indices.push_back(2);
        data.Indices.push_back(0); // Triangle 2: v0, v2, v3
        data.Indices.push_back(2);
        data.Indices.push_back(3);

        return std::make_unique<Mesh<ShadedVertex>>(std::move(data));
    }
}
