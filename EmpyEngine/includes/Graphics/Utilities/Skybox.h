#pragma once
#include "../Buffers/Mesh.h"

namespace Empy
{
    // skybox mesh type definition
    using SkyboxMesh = std::unique_ptr<Mesh<SkyboxVertex>>;

    // helps render a skybox mesh
    EMPY_INLINE void RenderSkyboxMesh(SkyboxMesh& mesh) 
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        mesh->Draw(GL_TRIANGLES); 
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_CULL_FACE);
    }

    // helps create a skybox mesh
    EMPY_INLINE SkyboxMesh CreateSkyboxMesh() 
    {
        MeshData<SkyboxVertex> data;

        std::vector<glm::vec3> positions = 
        {
            {-1.0f,  1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},

            {-1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f, -1.0f},
            {-1.0f,  1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f,  1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f,  1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},

            {-1.0f, -1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0F},
            {1.0f,  1.0f,  1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f, -1.0f,  1.0f},
            {-1.0f, -1.0f,  1.0f},

            {-1.0f,  1.0f, -1.0f},
            {1.0f,  1.0f, -1.0f},
            {1.0f,  1.0f,  1.0f},
            {1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f,  1.0f},
            {-1.0f,  1.0f, -1.0f},

            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f,  1.0f},
            {1.0f, -1.0f,  1.0f}
        };  

        // set veritces
        for (uint32_t i = 0; i < positions.size(); ++i) 
        {
            data.Vertices.push_back({ positions[i] });
        }      

        return std::make_unique<Mesh<SkyboxVertex>>(data);
    }
}
