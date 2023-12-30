#pragma once
#include "Buffers/Frame.h"
#include "Shaders/Final.h"
#include "Shaders/PBR.h"

namespace Empy
{
    struct GraphicsRenderer
    {               
        EMPY_INLINE GraphicsRenderer(int32_t width, int32_t height) 
        {
            // initialize opengl
            if(glewInit() != GLEW_OK) 
            {
                EMPY_FATAL("failed to init glew!");
                exit(EXIT_FAILURE);
            }
            glewExperimental = GL_TRUE;

            m_Final = std::make_unique<FinalShader>("Resources/Shaders/final.glsl");
            m_Pbr = std::make_unique<PbrShader>("Resources/Shaders/pbr.glsl");
            m_Frame = std::make_unique<FrameBuffer>(width, height);  
        }

        EMPY_INLINE void SetDirectLight(DirectLight& light, Transform3D& transform, uint32_t index) 
        {
            m_Pbr->SetDirectLight(light, transform, index);
        }

        EMPY_INLINE void SetPointLight(PointLight& light, Transform3D& transform, uint32_t index) 
        {
            m_Pbr->SetPointLight(light, transform, index);
        }

        EMPY_INLINE void SetSpotLight(SpotLight& light, Transform3D& transform, uint32_t index) 
        {
            m_Pbr->SetSpotLight(light, transform, index);
        }

        // --

        EMPY_INLINE void SetDirectLightCount(int32_t count)
        {
            m_Pbr->SetDirectLightCount(count);
        }

        EMPY_INLINE void SetPointLightCount(int32_t count)
        {
            m_Pbr->SetPointLightCount(count);
        }

        EMPY_INLINE void SetSpotLightCount(int32_t count)
        {
            m_Pbr->SetSpotLightCount(count);
        }
       
        // --

        EMPY_INLINE void Draw(Model3D& model, PbrMaterial& material, Transform3D& transform)
        {
            m_Pbr->Draw(model, material, transform);
        }

        EMPY_INLINE void SetCamera(Camera3D& camera, Transform3D& transform)
        {
            m_Pbr->SetCamera(camera, transform, m_Frame->Ratio());
        }
        
        EMPY_INLINE void Resize(int32_t width, int32_t height) 
        {
            m_Frame->Resize(width, height);            
        }

        // --

        EMPY_INLINE uint32_t GetFrame() 
        {
            return m_Frame->GetTexture();
        }
        
        EMPY_INLINE void ShowFrame()
        {
            m_Final->Show(m_Frame->GetTexture());
        }  

        EMPY_INLINE void NewFrame()
        {
            m_Frame->Begin();   
            m_Pbr->Bind();      
        }     

        EMPY_INLINE void EndFrame()
        {
            m_Pbr->Unbind();      
            m_Frame->End();
        }   

    private:
        std::unique_ptr<FrameBuffer> m_Frame;
        std::unique_ptr<FinalShader> m_Final;
        std::unique_ptr<PbrShader> m_Pbr;
    };
}