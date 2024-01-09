#pragma once
#include "Common/Core.h"

namespace Empy
{
    struct FrameBuffer 
    {
        EMPY_INLINE FrameBuffer(int32_t width, int32_t height):
        m_Width(width), m_Height(height)
        {
            glGenFramebuffers(1, &m_BufferID);
            glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

            CreateColorAttachment();
            CreateRenderBuffer();

            // Attachment Tagets
            uint32_t attachments[1] = 
            { 
                GL_COLOR_ATTACHMENT0 
            };

            glDrawBuffers(1, attachments);

            // check frame buffer
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
            { 
                EMPY_ERROR("glCheckFramebufferStatus() Failed!"); 
            }

            // unbind frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }              
              
        EMPY_INLINE void Resize(int32_t width, int32_t height) 
        {	
            // update size     
            m_Width = width;       
            m_Height = height;

            // Resize Color Attachment
            glBindTexture(GL_TEXTURE_2D, m_Color);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 
            m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);

            // Resize Depth Attachment
            glBindRenderbuffer(GL_RENDERBUFFER, m_Render);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);

            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }       

        EMPY_INLINE uint32_t GetTexture() 
        { 
            return m_Color; 
        } 

        EMPY_INLINE ~FrameBuffer() 
        {
            glDeleteTextures(1, &m_Color); 
            glDeleteRenderbuffers(1, &m_Render); 
            glDeleteFramebuffers(1, &m_BufferID); 
        }
                 
        EMPY_INLINE float Ratio() 
        { 
            return (float)m_Width/(float)m_Height; 
        }

        EMPY_INLINE int32_t Width() 
        { 
            return m_Width; 
        }

        EMPY_INLINE int32_t Height() 
        { 
            return m_Height; 
        }

        EMPY_INLINE void Begin() 
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);   
            glViewport(0, 0, m_Width, m_Height);
            glClearColor(0, 0, 0, 1);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST); 
            glEnable(GL_SAMPLES);  	
        }

        EMPY_INLINE void End() 
        {
            glDisable(GL_SAMPLES);  	
            glDisable(GL_DEPTH_TEST); 
            glBindFramebuffer(GL_FRAMEBUFFER, 0);             
        }
    
    private:
        EMPY_INLINE void CreateColorAttachment() 
        {
            glGenTextures(1, &m_Color);
            glBindTexture(GL_TEXTURE_2D, m_Color);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Color, 0);
        }

        EMPY_INLINE void CreateRenderBuffer() 
        {
            glGenRenderbuffers(1, &m_Render);
            glBindRenderbuffer(GL_RENDERBUFFER, m_Render);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Render);
        }

    private:
        uint32_t m_BufferID = 0u;
        uint32_t m_Render = 0u;
        uint32_t m_Color = 0u;
        int32_t m_Height = 0;
        int32_t m_Width = 0;
    };
}