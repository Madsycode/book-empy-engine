#pragma once
#include "Interface.h"

namespace Empy
{
    struct Application : AppInterface
    {
        EMPY_INLINE Application() 
        {
            // intialize app context
            m_Context = new AppContext();
            m_LayerID = TypeID<Application>();

            // attach window resize event callback
            AttachCallback<WindowResizeEvent>([this] (auto e) 
            {
                m_Context->Renderer->Resize(e.Width, e.Height);
            });
        }

        EMPY_INLINE ~Application() 
        {
            EMPY_DELETE(m_Context);
        }

        EMPY_INLINE void RunContext() 
        {
            // load textures
            auto skymap = std::make_shared<Texture2D>("Resources/Textures/HDRs/Sky.hdr", true, true);

            // auto roughness = std::make_shared<Texture2D>("Resources/Textures/Marble/Roughness.png");
            // auto albedo = std::make_shared<Texture2D>("Resources/Textures/Marble/Albedo.png");
            // auto normal = std::make_shared<Texture2D>("Resources/Textures/Marble/Normal.png");

            // load models
            auto sphereModel = std::make_shared<Model>("Resources/Models/sphere.fbx");
            auto cubeModel = std::make_shared<Model>("Resources/Models/cube.fbx");

            // create scene camera
            auto camera = CreateEntt<Entity>();                    
            camera.Attach<TransformComponent>().Transform.Translate.z = 3.0f;
            camera.Attach<CameraComponent>();

            // skybox entity
            auto skybox = CreateEntt<Entity>();                    
            skybox.Attach<TransformComponent>();
            skybox.Attach<SkyboxComponent>();

            // create point light 1      
            auto slight = CreateEntt<Entity>();                    
            slight.Attach<DirectLightComponent>().Light.Intensity = 5.0f;
            auto& stp = slight.Attach<TransformComponent>().Transform;
            stp.Rotation = glm::vec3(0.0f, 0.0f, -1.0f); 

            // create sphere entity
            auto sphere = CreateEntt<Entity>();
            auto& mod = sphere.Attach<ModelComponent>();
            mod.Model = sphereModel;
            mod.Material.Emissive = glm::vec3(1.0f);
            mod.Material.Albedo = glm::vec3(0.8f, 0.1f, 0.8f);
            sphere.Attach<TransformComponent>().Transform.Translate.x = -1.0f;

            // create cube entity
            auto cube = CreateEntt<Entity>();
            auto& mod1 = cube.Attach<ModelComponent>();
            mod1.Model = cubeModel;
            mod1.Material.Albedo = glm::vec3(0.1f, 0.0f, 0.5f);
            cube.Attach<TransformComponent>().Transform.Translate.x = 1.0f;

            // generate enviroment maps
            EnttView<Entity, SkyboxComponent>([this, &skymap] (auto entity, auto& comp) 
            {      
                m_Context->Renderer->InitSkybox(comp.Sky, skymap, 2048);
            });

            // application main loop
            while(m_Context->Window->PollEvents())
            {
                // render scene depth
                RenderSceneDepth();

                // start new frame
                m_Context->Renderer->NewFrame(); 
                                 
                // set shader camera
                EnttView<Entity, CameraComponent>([this] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->SetCamera(comp.Camera, transform);
                });

                // set shader point lights
                int32_t lightCounter = 0u;
                EnttView<Entity, PointLightComponent>([this, &lightCounter] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->SetPointLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                }); 
                // set number of point lights
                m_Context->Renderer->SetPointLightCount(lightCounter);

                // set shader direct. lights
                lightCounter = 0;
                EnttView<Entity, DirectLightComponent>([this, &lightCounter] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->SetDirectLight(comp.Light, transform, lightCounter);
                    lightCounter++;            
                }); 
                // set number of direct lights
                m_Context->Renderer->SetDirectLightCount(lightCounter);

                // set shader spot lights
                lightCounter = 0;
                EnttView<Entity, SpotLightComponent>([this, &lightCounter] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->SetSpotLight(comp.Light, transform, lightCounter);
                    lightCounter++;
                }); 
                // set number of spot lights
                m_Context->Renderer->SetSpotLightCount(lightCounter);

                // render skybox
                EnttView<Entity, SkyboxComponent>([this] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->DrawSkybox(comp.Sky, transform);
                });

                // render models
                EnttView<Entity, ModelComponent>([this] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->Draw(comp.Model, comp.Material, transform);    
                });  

                // end frame                
                m_Context->Renderer->EndFrame();            

                // update layers
                for(auto layer : m_Context->Layers)
                {
                    layer->OnUpdate();
                }              

                // show frame to screen
                m_Context->Renderer->ShowFrame();
            }
        }

    private:
        EMPY_INLINE void RenderSceneDepth()
        {        
            EnttView<Entity, DirectLightComponent>([this] (auto light, auto&) 
            {
                // light direction
                auto& lightDir = light.template Get<TransformComponent>().Transform.Rotation;
                float delta = 0.0166666f;

                if(m_Context->Window->IsKey(GLFW_KEY_A))
                    lightDir.x -= delta;
                if(m_Context->Window->IsKey(GLFW_KEY_D))
                    lightDir.x += delta;
                if(m_Context->Window->IsKey(GLFW_KEY_W))
                    lightDir.y -= delta;
                if(m_Context->Window->IsKey(GLFW_KEY_S))
                    lightDir.y += delta;
                if(m_Context->Window->IsKey(GLFW_KEY_E))
                    lightDir.z -= delta;
                if(m_Context->Window->IsKey(GLFW_KEY_R))
                    lightDir.z += delta;

                // begin rendering
                m_Context->Renderer->BeginShadowPass(lightDir);

                // render depth 
                EnttView<Entity, ModelComponent>([this, &lightDir] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->DrawDepth(comp.Model, transform);  

                    float delta = 0.0166666f;
                    if(4 == (uint32_t)entity.ID())
                    {
                        if(m_Context->Window->IsKey(GLFW_KEY_LEFT))
                            transform.Translate.x -= delta;
                        if(m_Context->Window->IsKey(GLFW_KEY_RIGHT))
                            transform.Translate.x += delta;
                        if(m_Context->Window->IsKey(GLFW_KEY_UP))
                            transform.Translate.y += delta;
                        if(m_Context->Window->IsKey(GLFW_KEY_DOWN))
                            transform.Translate.y -= delta;
                        if(m_Context->Window->IsKey(GLFW_KEY_KP_0))
                            transform.Translate.z -= delta;
                        if(m_Context->Window->IsKey(GLFW_KEY_KP_1))
                            transform.Translate.z += delta;                                                 
                    }
                }); 

                // ffinalize frame
                m_Context->Renderer->EndShadowPass();
            });
        }
    };
}