#pragma once
#include "Shader.h"
#include "../Utilities/Quad.h"

namespace Empy
{
    struct FinalShader : Shader 
    { 
        EMPY_INLINE FinalShader(const std::string& filename): Shader(filename) 
        {
            u_Bloom = glGetUniformLocation(m_ShaderID, "u_bloom");
            u_Map = glGetUniformLocation(m_ShaderID, "u_map");
            m_Quad = CreateQuad2D();
        } 

        EMPY_INLINE void Show(uint32_t map, uint32_t bloom) 
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        }

    private:
        uint32_t u_Bloom = 0u;
        uint32_t u_Map = 0u;
        Quad2D m_Quad;
    }; 
}