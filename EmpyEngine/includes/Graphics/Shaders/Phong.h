#pragma once
#include "Shader.h"
#include "../Utilities/Data.h"

namespace Empy
{
    struct PhongShader : Shader 
    {        
        EMPY_INLINE PhongShader(const std::string& filename): Shader(filename) 
        {
            u_Model = glGetUniformLocation(m_ShaderID, "u_model");
            u_View = glGetUniformLocation(m_ShaderID, "u_view");
            u_Proj = glGetUniformLocation(m_ShaderID, "u_proj");
        } 

        EMPY_INLINE void SetCamera(Camera3D& camera, Transform3D& transform, float ratio) 
        {
            glUniformMatrix4fv(u_Proj, 1, GL_FALSE, glm::value_ptr(camera.Projection(ratio)));
            glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(camera.View(transform)));
        }   

        EMPY_INLINE void Draw(Mesh3D& mesh, Transform3D& transform)
        {
            glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));            
            mesh->Draw(GL_TRIANGLES);        
        }

    private:        
        uint32_t u_Model = 0u;
        uint32_t u_View = 0u;
        uint32_t u_Proj = 0u;
    };   
}