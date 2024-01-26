#pragma once
#include "Shader.h"
#include "../Utilities/Skybox.h"

namespace Empy
{
    struct SkyMapShader : Shader 
    { 
        EMPY_INLINE SkyMapShader(const std::string& path): Shader(path) 
        {
            u_View = glGetUniformLocation(m_ShaderID, "u_view");
            u_Proj = glGetUniformLocation(m_ShaderID, "u_proj");
            u_Map = glGetUniformLocation(m_ShaderID, "u_map");
        } 

        EMPY_INLINE uint32_t Generate(Texture2D& texture, SkyboxMesh& mesh, int32_t size) 
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

            // projection matrix
            glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

            // bind shader
            glUseProgram(m_ShaderID); 

            // set projection matrix
            glUniformMatrix4fv(u_Proj, 1, GL_FALSE, glm::value_ptr(proj));

            // set texture source
            texture.Use(u_Map, 0);

            // generate cube map
            uint32_t cubeMap = 0u;
            glGenTextures(1, &cubeMap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

            // set cube map faces size
            for (uint32_t i = 0; i < 6; ++i) 
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
                GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, NULL);
            }

            // set cube map sampling parameters
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            // target frame and render buffer
            uint32_t FBO = 0u, RBO = 0u;            
            glGenFramebuffers(1, &FBO);
            glGenRenderbuffers(1, &RBO);

            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

            // set view port
            glViewport(0, 0, size, size);

            for (uint32_t i = 0; i < 6; ++i) 
            {   
                // set current face view matrix
                glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(views[i]));

                // set cube map current face 
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap, 0);

                // clear frame buffer
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // render face
                RenderSkyboxMesh(mesh);
            }

            // generate cubemap mipmap levels
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

            // unbind shader, buffer
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glUseProgram(0);

            // delete buffers 
            glDeleteRenderbuffers(1, &RBO);
            glDeleteFramebuffers(1, &FBO);
            return cubeMap;
        }

    private:
        uint32_t u_View = 0u;
        uint32_t u_Proj = 0u;
        uint32_t u_Map = 0u;
    }; 
}