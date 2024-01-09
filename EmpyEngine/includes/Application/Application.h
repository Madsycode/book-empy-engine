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
            // load environment map
            auto skymap = std::make_shared<Texture2D>("Resources/Textures/HDRs/Sky.hdr", true, true);

            // load textures
            auto roughness = std::make_shared<Texture2D>("Resources/Textures/Marble/Roughness.png");
            auto albedo = std::make_shared<Texture2D>("Resources/Textures/Marble/Albedo.png");
            auto normal = std::make_shared<Texture2D>("Resources/Textures/Marble/Normal.png");

            // load models
            auto sphereModel = std::make_shared<Model>("Resources/Models/sphere.fbx");
            auto cubeModel = std::make_shared<Model>("Resources/Models/cube.fbx");

            // create scene camera
            auto camera = CreateEntt<Entity>();                    
            camera.Attach<TransformComponent>().Transform.Translate.z = 3.0f;
            camera.Attach<CameraComponent>();

            // create environment entity
            auto skybox = CreateEntt<Entity>();                    
            skybox.Attach<TransformComponent>();
            skybox.Attach<SkyboxComponent>();

            // create point light 1      
            auto slight = CreateEntt<Entity>();                    
            slight.Attach<DirectLightComponent>().Light.Intensity = 5.0f;
            auto& stp = slight.Attach<TransformComponent>().Transform;
            stp.Rotation = glm::vec3(0.0f, 0.0f, -1.0f);
            stp.Translate.z = 1.0f;

            // create cube entity
            auto cube = CreateEntt<Entity>();
            cube.Attach<TransformComponent>().Transform.Scale *= 2.0f;
            auto& mod = cube.Attach<ModelComponent>();
            mod.Model = sphereModel;

            mod.Material.Albedo = glm::vec3(0.0f); 
            mod.Material.Roughness = 0.1f;      
            mod.Material.Metallic = 0.25f;         

            mod.Material.RoughnessMap = roughness; 
            mod.Material.AlbedoMap = albedo;      
            mod.Material.NormalMap = normal;       

            // generate enviroment maps
            EnttView<Entity, SkyboxComponent>([this, &skymap] (auto entity, auto& comp) 
            {      
                m_Context->Renderer->InitSkybox(comp.Sky, skymap, 2048);
            });

            // application main loop
            while(m_Context->Window->PollEvents())
            {
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
                    transform.Rotation.y += 0.16666f;                                                 
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
    };
}