#pragma once
#include "Common/Core.h"
#include <stb_image.h>

namespace Empy
{  
    struct Texture2D 
    {   
        EMPY_INLINE Texture2D(const std::string& path, bool isHDR, bool flipY) 
        { 
            Load(path, isHDR, flipY); 
        }

        EMPY_INLINE Texture2D(const std::string& path) { Load(path); }
        EMPY_INLINE ~Texture2D() { glDeleteTextures(1, &m_ID); }
        EMPY_INLINE Texture2D() = default;

        EMPY_INLINE bool Load(const std::string& path, bool isHDR = false, bool flipY = true) 
        {
            // flip y axis (common)
            stbi_set_flip_vertically_on_load(flipY);
            void* pixels = nullptr;

            // load texture data
            if (isHDR) 
            {
                int32_t channels;
				pixels = stbi_loadf(path.c_str(), &m_Width, &m_Height, &channels, 0);
			}
            else
            {
                pixels = stbi_load(path.c_str(), &m_Width, &m_Height, nullptr, 4);
            }

            // check pixels
            if(pixels == nullptr) 
            { 
                EMPY_ERROR("failed to load texture!");
                return false; 
            }

            // create texture
            glGenTextures(1, &m_ID);
            glBindTexture(GL_TEXTURE_2D, m_ID);            

            // load texture to gpu
            if (isHDR) 
            {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, 
                m_Height, 0, GL_RGB, GL_FLOAT, (float*)pixels);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 
                0, GL_RGBA, GL_UNSIGNED_BYTE, (uint32_t*)pixels);
			}

            // free allocated memory
            stbi_image_free(pixels);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glGenerateMipmap(GL_TEXTURE_2D);      

            glBindTexture(GL_TEXTURE_2D, 0);  
            return true;     
        } 
      
        EMPY_INLINE operator uint32_t() const { return m_ID; }
        EMPY_INLINE int32_t Height() const { return m_Height; }
        EMPY_INLINE int32_t Width() const { return m_Width; }
        EMPY_INLINE uint32_t ID() const { return m_ID; }

        EMPY_INLINE void Bind() { glBindTexture(GL_TEXTURE_2D, m_ID); }
        EMPY_INLINE void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

        EMPY_INLINE void Use(uint32_t uniform, int32_t unit) 
        { 
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, m_ID);
            glUniform1i(uniform, unit); 
        }


    private:
        int32_t m_Height = 0;
        int32_t m_Width = 0;
        uint32_t m_ID = 0u;
    };

    using Texture = std::shared_ptr<Texture2D>;
}