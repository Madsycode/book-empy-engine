#pragma once
#include "../Utilities/Data.h"
#include "../Textures/Texture.h"

namespace Empy
{
    struct Shader 
    {
        EMPY_INLINE Shader(const std::string& filename) 
        {
            m_ShaderID = Load(filename);
        }

        EMPY_INLINE virtual ~Shader() 
        {
            glDeleteProgram(m_ShaderID);
        }

        EMPY_INLINE void Unbind() 
        { 
            glUseProgram(0); 
        } 

        EMPY_INLINE void Bind() 
        { 
            glUseProgram(m_ShaderID); 
        }  
    
    private:
        EMPY_INLINE uint32_t Build(const char* src, uint32_t type) 
        {
            uint32_t shaderID = glCreateShader(type);
            glShaderSource(shaderID, 1, &src, NULL);
            glCompileShader(shaderID);

            char error[512];      
            int32_t status = 0;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

            if (!status) {
                glGetShaderInfoLog(shaderID, 512, NULL, error);
                throw std::runtime_error(error);
                glDeleteShader(shaderID);
                shaderID = 0u;
            }

            return shaderID;
        } 

        EMPY_INLINE uint32_t Link(uint32_t vert, uint32_t frag) 
        {            
            uint32_t programID = glCreateProgram();
            glAttachShader(programID, vert);
            glAttachShader(programID, frag);
            glLinkProgram(programID);

            char error[512];
            int32_t status = 0;
            glGetProgramiv(programID, GL_LINK_STATUS, &status);

            if (!status) {
                glGetProgramInfoLog(programID, 512, NULL, error);
                throw std::runtime_error(error);
                glDeleteProgram(programID);
            }

            glDeleteShader(vert);
            glDeleteShader(frag);	

            return programID;
        }

        EMPY_INLINE uint32_t Load(const std::string& filename) 
        {
            std::ifstream fs;
            fs.exceptions(std::ifstream::failbit | std::fstream::badbit);
            try 
            {
                bool loading_vtx_source = true;
                fs.open(filename);

                std::string line;
                std::string vtxSource;
                std::string fragSource;

                // load vtx & frag source
                while(getline(fs, line)) 
                {
                    if(loading_vtx_source) 
                    {
                        if(line.compare("++VERTEX++")) 
                        { 
                            vtxSource.append(line + "\n");
                            continue;
                        }						
                        loading_vtx_source = false;
                        continue;
                    }
                    else 
                    {
                        if(!line.compare("++FRAGMENT++")) 
                        { 
                            break; 
                        }						
                        fragSource.append(line + "\n");
                    }
                }
                fs.close();

                uint32_t vtxShader = Build(vtxSource.c_str(), GL_VERTEX_SHADER);
                uint32_t fragShader = Build(fragSource.c_str(), GL_FRAGMENT_SHADER);
                return Link(vtxShader, fragShader);
            }
            catch (const std::exception& e) 
            {	
                EMPY_ERROR("Load('{}') Failed: {}", filename, e.what());	
            }
            return 0;
        }  
                      
    protected:
        uint32_t m_ShaderID = 0u;
    };    
}