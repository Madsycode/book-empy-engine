#pragma once
#include "Shader.h"
#include "../Utilities/Skybox.h"

namespace Empy
{
    struct PrefilteredShader : Shader 
    { 
        EMPY_INLINE PrefilteredShader(const std::string& path): Shader(path) 
        {
            u_Roughness = glGetUniformLocation(m_ShaderID, "u_roughness");            
            u_CubeMap = glGetUniformLocation(m_ShaderID, "u_cubemap");
            u_View = glGetUniformLocation(m_ShaderID, "u_view");
            u_Proj = glGetUniformLocation(m_ShaderID, "u_proj");
        } 

        EMPY_INLINE uint32_t Generate(uint32_t skyCubMap, SkyboxMesh& mesh, int32_t size) 
        {            
            // view matrices
            glm::mat4 views[] = 
            {
                glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
            };

            // projection
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

            // generate cube map
            uint32_t prefilteredMap = 0u;
            glGenTextures(1, &prefilteredMap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMap);

            // init size
            for (uint32_t i = 0; i < 6; ++i) 
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
                GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glUseProgram(m_ShaderID); 
            glUniformMatrix4fv(u_Proj, 1, GL_FALSE, glm::value_ptr(projection));

            // bind skybox cube map
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyCubMap);
            glUniform1i(u_CubeMap, 0); 

            
            uint32_t FBO, RBO = 0u;
            glGenFramebuffers(1, &FBO);
            glGenRenderbuffers(1, &RBO);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);

            // number of mip levels
            uint32_t nbrMipLevels = 5;

            // loop for each mip level
            for (uint32_t mip = 0; mip < nbrMipLevels; ++mip) 
            {
                // reisze framebuffer according to mip-level.
                int32_t mipWidth = (int)(size * std::pow(0.5, mip));
                int32_t mipHeight = (int)(size * std::pow(0.5, mip));

                glBindRenderbuffer(GL_RENDERBUFFER, RBO);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);                
                glViewport(0, 0, mipWidth, mipHeight);

                float roughness = (float)mip / (float)(nbrMipLevels - 1);
                glUniform1f(u_Roughness, roughness);

                for (uint32_t i = 0; i < 6; ++i) 
                {
                    glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(views[i]));
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredMap, mip);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    RenderSkyboxMesh(mesh);
                }
            }

            // unbind shader, buffer
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glUseProgram(0);

            // delete fbo & rbo
            glDeleteRenderbuffers(1, &RBO);
            glDeleteFramebuffers(1, &FBO);
            return prefilteredMap;
        }

    private:
        uint32_t u_Roughness = 0u;        
        uint32_t u_CubeMap = 0u;
        uint32_t u_View = 0u;
        uint32_t u_Proj = 0u;
    }; 
}