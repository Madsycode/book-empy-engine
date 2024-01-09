#pragma once
#include "Shaders/Prefiltered.h"
#include "Shaders/Irradiance.h"
#include "Shaders/Skybox.h"
#include "Shaders/SkyMap.h"
#include "Buffers/Frame.h"
#include "Shaders/Final.h"
#include "Shaders/BRDF.h"
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

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            m_Prefil = std::make_unique<PrefilteredShader>("Resources/Shaders/prefiltered.glsl");
            m_Irrad = std::make_unique<IrradianceShader>("Resources/Shaders/irradiance.glsl");
            m_Skybox = std::make_unique<SkyboxShader>("Resources/Shaders/skybox.glsl");
            m_SkyMap = std::make_unique<SkyMapShader>("Resources/Shaders/skymap.glsl");            
            m_Brdf = std::make_unique<BrdfShader>("Resources/Shaders/brdf.glsl");

            m_Final = std::make_unique<FinalShader>("Resources/Shaders/final.glsl");
            m_Pbr = std::make_unique<PbrShader>("Resources/Shaders/pbr.glsl");

            m_Frame = std::make_unique<FrameBuffer>(width, height);  
            m_SkyboxMesh = CreateSkyboxMesh();
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

        EMPY_INLINE void InitSkybox(Skybox& sky, Texture& texture, int32_t size)
        {
            sky.BrdfMap = m_Brdf->Generate(512);
            sky.CubeMap = m_SkyMap->Generate(texture, m_SkyboxMesh, size);            
            sky.IrradMap = m_Irrad->Generate(sky.CubeMap, m_SkyboxMesh, 32);            
            sky.PrefilMap = m_Prefil->Generate(sky.CubeMap, m_SkyboxMesh, 128);                      
        }

        EMPY_INLINE void DrawSkybox(Skybox& sky, Transform3D& transform)
        {
            m_Skybox->Draw(m_SkyboxMesh, sky.CubeMap, transform);
            m_Pbr->SetEnvMaps(sky.IrradMap, sky.PrefilMap, sky.BrdfMap);            
        }

        EMPY_INLINE void SetCamera(Camera3D& camera, Transform3D& transform)
        {
            float aspect = m_Frame->Ratio();
            m_Pbr->SetCamera(camera, transform, aspect);

            // binds skybox shader ad set mvp
            m_Skybox->SetCamera(camera, transform, aspect);

            // rebind pbr shader again
            m_Pbr->Bind();      
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
        std::unique_ptr<PrefilteredShader> m_Prefil;        
        std::unique_ptr<IrradianceShader> m_Irrad;
        std::unique_ptr<SkyboxShader> m_Skybox;        
        std::unique_ptr<SkyMapShader> m_SkyMap;
        std::unique_ptr<FrameBuffer> m_Frame;
        std::unique_ptr<FinalShader> m_Final;
        std::unique_ptr<BrdfShader> m_Brdf;
        std::unique_ptr<PbrShader> m_Pbr;        
        SkyboxMesh m_SkyboxMesh;
    };
}