#pragma once
#include "Shader.h"

namespace Empy
{
    struct ShadowShader : Shader 
    { 
        EMPY_INLINE ShadowShader(const std::string& path): Shader(path) 
        {
            u_LightSpace = glGetUniformLocation(m_ShaderID, "u_lightSpace");
            u_Model = glGetUniformLocation(m_ShaderID, "u_model");            

            // create depth texture
            glGenTextures(1, &m_DepthMap);
            glBindTexture(GL_TEXTURE_2D, m_DepthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
            MapSize, MapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

            // set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // create frame buffer
            glGenFramebuffers(1, &m_FrameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

            // attach to frame buffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);

            // no drawing nor reading 
            glDrawBuffer(GL_NONE); 
            glReadBuffer(GL_NONE); 

            // check frame buffer
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
            { 
                EMPY_ERROR("CreateDepthBuffer() Failed!"); 
            }

            // unbind frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);      
        } 

        EMPY_INLINE void Draw(Model3D& model, Transform3D& transform)
        {
            glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(transform.Matrix()));
            model->Draw(GL_TRIANGLES);
        }

        EMPY_INLINE void BeginFrame(const glm::mat4& lightSpaceMtx)
        {
            // bind shadow shader
            glUseProgram(m_ShaderID);

            // set view projection matrix
            glUniformMatrix4fv(u_LightSpace, 1, GL_FALSE, 
            glm::value_ptr(lightSpaceMtx));  

            // bind target frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);   

            // set viewport a clear buffer
            glViewport(0, 0, MapSize, MapSize);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST); 
        }

        EMPY_INLINE uint32_t GetDepthMap()
        {
            return m_DepthMap;
        }

        EMPY_INLINE void EndFrame()
        {
            glDisable(GL_DEPTH_TEST); 
            glBindFramebuffer(GL_FRAMEBUFFER, 0);  
            glUseProgram(0);
        }

        EMPY_INLINE ~ShadowShader()
        {
            glDeleteFramebuffers(1, &m_FrameBuffer); 
            glDeleteTextures(1, &m_DepthMap); 
        }

    private:
        uint32_t m_FrameBuffer = 0u;
        uint32_t m_DepthMap = 0u;
        int32_t MapSize = 1024;

        uint32_t u_LightSpace = 0u;
        uint32_t u_Model = 0u;        
    }; 
}