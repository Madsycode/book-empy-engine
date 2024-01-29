#pragma once
#include "Shaders/Prefiltered.h"
#include "Shaders/Irradiance.h"
#include "Shaders/Skybox.h"
#include "Shaders/SkyMap.h"
#include "Shaders/Shadow.h"
#include "Buffers/Frame.h"
#include "Shaders/Bloom.h"
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

            m_Bloom = std::make_unique<BloomShader>("Resources/Shaders/bloom.glsl", width, height);
            m_Final = std::make_unique<FinalShader>("Resources/Shaders/final.glsl", width, height);

            m_Prefil = std::make_unique<PrefilteredShader>("Resources/Shaders/prefiltered.glsl");
            m_Irrad = std::make_unique<IrradianceShader>("Resources/Shaders/irradiance.glsl");
            m_Skybox = std::make_unique<SkyboxShader>("Resources/Shaders/skybox.glsl");
            m_SkyMap = std::make_unique<SkyMapShader>("Resources/Shaders/skymap.glsl");            
            m_Brdf = std::make_unique<BrdfShader>("Resources/Shaders/brdf.glsl");

            m_Shadow = std::make_unique<ShadowShader>("Resources/Shaders/shadow.glsl");
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

        EMPY_INLINE void Animate(Model3D model, float dt) 
        {
            if(auto joints = model->Animate(dt))
            {
                m_Pbr->SetJoints(*joints);
            }
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

        EMPY_INLINE void Draw(Model3D& model, Material& material, Transform3D& transform)
        {
            m_Pbr->Draw(model, material, transform);
        }

        EMPY_INLINE void DrawDepth(Model3D& model, Transform3D& transform)
        {
            m_Shadow->Draw(model, transform);
        }

        EMPY_INLINE void InitSkybox(Skybox& skybox, Texture2D& texture, int32_t size)
        {
            skybox.BrdfMap = m_Brdf->Generate(512);
            skybox.CubeMap = m_SkyMap->Generate(texture, m_SkyboxMesh, size);            
            skybox.IrradMap = m_Irrad->Generate(skybox.CubeMap, m_SkyboxMesh, 32);            
            skybox.PrefilMap = m_Prefil->Generate(skybox.CubeMap, m_SkyboxMesh, 128);                      
        }

        EMPY_INLINE void DrawSkybox(Skybox& skybox, Transform3D& transform)
        {            
            m_Skybox->Draw(m_SkyboxMesh, skybox.CubeMap, transform);
            m_Pbr->SetEnvMaps(skybox.IrradMap, skybox.PrefilMap, 
            skybox.BrdfMap, m_Shadow->GetDepthMap());   
        }

        EMPY_INLINE void SetCamera(Camera3D& camera, Transform3D& transform)
        {
            // frame aspect ratio
            float aspect = m_Frame->Ratio();

            // binds skybox shader ad set mvp
            m_Skybox->SetCamera(camera, transform, aspect);

            // rebind pbr shader again
            m_Pbr->Bind();      
            m_Pbr->SetCamera(camera, transform, aspect);
        }
               
        EMPY_INLINE void Resize(int32_t width, int32_t height) 
        {
            m_Frame->Resize(width, height);      
            m_Bloom->Resize(width, height);      
            m_Final->Resize(width, height);      
        }

        // --

        EMPY_INLINE void BeginShadowPass(const glm::vec3& LightDir)
        {            
            // prepare projection and view mtx
            static auto proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
            auto view = glm::lookAt(LightDir, glm::vec3(0.0f, 0.0f,  0.0f), 
            glm::vec3(0.0f, 1.0f,  0.0f));  

            // compute light space
            auto lightSpaceMtx = (proj * view);

            // set pbr shader light space mtx and depth map
            m_Pbr->Bind();
            m_Pbr->SetLightSpaceMatrix(lightSpaceMtx);

            // begin depth rendering
            m_Shadow->BeginFrame(lightSpaceMtx);   
        } 

        EMPY_INLINE void EndShadowPass()
        {
            m_Shadow->EndFrame();
        } 

        // --

        EMPY_INLINE uint32_t GetFrame() 
        {
            //return m_Frame->GetTexture();
            return m_Final->GetMap();
        }
        
        EMPY_INLINE void ShowFrame(bool useFBO)
        {
            glViewport(0, 0, m_Frame->Width(), m_Frame->Height());         
            m_Final->Render(m_Frame->GetTexture(), m_Bloom->GetMap(), useFBO);
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

            // post-processing
            m_Bloom->Compute(m_Frame->GetBrightnessMap(), 10);
        }   

    private:
        std::unique_ptr<PrefilteredShader> m_Prefil;        
        std::unique_ptr<IrradianceShader> m_Irrad;
        std::unique_ptr<SkyboxShader> m_Skybox;        
        std::unique_ptr<ShadowShader> m_Shadow;
        std::unique_ptr<SkyMapShader> m_SkyMap;
        std::unique_ptr<BloomShader> m_Bloom;
        std::unique_ptr<FinalShader> m_Final;
        std::unique_ptr<BrdfShader> m_Brdf;
        std::unique_ptr<PbrShader> m_Pbr;    

        std::unique_ptr<FrameBuffer> m_Frame;
        SkyboxMesh m_SkyboxMesh;
    };
}