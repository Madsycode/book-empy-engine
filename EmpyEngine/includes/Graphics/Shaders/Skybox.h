#pragma once
#include "Shader.h"
#include "../Utilities/Skybox.h"

namespace Empy
{
    struct SkyboxShader : Shader 
    { 
        EMPY_INLINE SkyboxShader(const std::string& path): Shader(path) 
        {
            u_Model = glGetUniformLocation(m_ShaderID, "u_model");            
            u_View = glGetUniformLocation(m_ShaderID, "u_view");
            u_Proj = glGetUniformLocation(m_ShaderID, "u_proj");
            u_Map = glGetUniformLocation(m_ShaderID, "u_map");
        } 

        EMPY_INLINE void SetCamera(Camera3D& camera, Transform3D& transform, float ratio) 
        {
            glUseProgram(m_ShaderID); 
            glUniformMatrix4fv(u_Proj, 1, GL_FALSE, glm::value_ptr(camera.Projection(ratio)));
            glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(camera.View(transform)));
        } 

        EMPY_INLINE void Draw(SkyboxMesh& mesh, uint32_t cubeMap, Transform3D& transform) 
        {
            glm::mat4 model = glm::toMat4(glm::quat(glm::radians(transform.Rotation)));

            glUseProgram(m_ShaderID); 
            glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(model));
            glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
			glUniform1i(u_Map, 0);
            RenderSkyboxMesh(mesh);
            glUseProgram(0); 
        }

    private:
        uint32_t u_Model = 0u;        
        uint32_t u_View = 0u;
        uint32_t u_Proj = 0u;
        uint32_t u_Map = 0u;
    }; 
}