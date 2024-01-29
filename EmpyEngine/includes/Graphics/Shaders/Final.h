#pragma once
#include "Shader.h"
#include "../Utilities/Quad.h"

namespace Empy
{
    struct FinalShader : Shader 
    { 
        EMPY_INLINE FinalShader(const std::string& filename, int32_t width, int32_t height): 
            Shader(filename) 
        {
            u_Bloom = glGetUniformLocation(m_ShaderID, "u_bloom");
            u_Map = glGetUniformLocation(m_ShaderID, "u_map");
            CreateBuffer(width, height);
            m_Quad = CreateQuad2D();
        } 

        EMPY_INLINE ~FinalShader()
        {
            glDeleteTextures(1, &m_Final); 
            glDeleteFramebuffers(1, &m_FBO); 
        }

        EMPY_INLINE void Render(uint32_t map, uint32_t bloom, bool useFBO) 
        {
            glBindFramebuffer(GL_FRAMEBUFFER, (useFBO) ? 0 : m_FBO);
            glClear(GL_COLOR_BUFFER_BIT); 
            glClearColor(0, 0, 0, 1);
            glUseProgram(m_ShaderID); 

            // set color map
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, map);
            glUniform1i(u_Map, 0);

            // set bloom map
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, bloom);
            glUniform1i(u_Bloom, 1);

            // render quad
            m_Quad->Draw(GL_TRIANGLES);
            glUseProgram(0); 

            // bind default fbo
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        EMPY_INLINE void Resize(int32_t width, int32_t height)
        {
            glBindTexture(GL_TEXTURE_2D, m_Final);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 
            width, height, 0, GL_RGBA, GL_FLOAT, NULL);            
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        EMPY_INLINE uint32_t GetMap()
        {
            return m_Final;
        }

    private:
        EMPY_INLINE void CreateBuffer(int32_t width, int32_t height) 
        {
            // create frame buffer
            glGenFramebuffers(1, &m_FBO);
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

            // create attachment
            glGenTextures(1, &m_Final);
            glBindTexture(GL_TEXTURE_2D, m_Final);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Final, 0);

            // check frame buffer
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
            { 
                EMPY_ERROR("glCheckFramebufferStatus() Failed!"); 
            }

            // unbind frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

    private:
        uint32_t m_Final = 0u;
        uint32_t m_FBO = 0u;

        uint32_t u_Bloom = 0u;
        uint32_t u_Map = 0u;

        Quad2D m_Quad;
    }; 
}