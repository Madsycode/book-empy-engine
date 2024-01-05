#pragma once
#include "Interface.h"

namespace Empy
{
    struct Application : AppInterface
    {
        EMPY_INLINE Application() 
        {
            // intialize application context
            m_LayerID = TypeID<Application>();
            m_Context = new AppContext();

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

            // create point light 1      
            auto slight = CreateEntt<Entity>();                    
            slight.Attach<DirectLightComponent>().Light.Intensity = 5.0f;
            auto& stp = slight.Attach<TransformComponent>().Transform;
            stp.Rotation = glm::vec3(0.0f, 0.0f, -2.0f);
            stp.Translate.z = 1.0f;

            // create cube entity
            auto cube = CreateEntt<Entity>();
            cube.Attach<TransformComponent>().Transform.Scale *= 2.5f;
            auto& mod = cube.Attach<ModelComponent>();
            mod.Model = sphereModel;

            mod.Material.Metallic = 0.25f;         
            mod.Material.Roughness = 0.1f;      
            mod.Material.Albedo = glm::vec3(0.0f); 

            mod.Material.RoughnessMap = roughness; 
            mod.Material.AlbedoMap = albedo;      
            mod.Material.NormalMap = normal;       


            // application main loop
            while(m_Context->Window->PollEvents())
            {
                // start new frame
                m_Context->Renderer->NewFrame(); 
                 
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

                // set shader camera
                EnttView<Entity, CameraComponent>([this] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->SetCamera(comp.Camera, transform);
                });

                // render models
                EnttView<Entity, ModelComponent>([this] (auto entity, auto& comp) 
                {      
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    m_Context->Renderer->Draw(comp.Model, comp.Material, transform);                
                    
                    if(m_Context->Window->IsKey(GLFW_KEY_A))
                        transform.Translate.x -= 0.05f;
                    if(m_Context->Window->IsKey(GLFW_KEY_D))
                        transform.Translate.x += 0.05f;
                    if(m_Context->Window->IsKey(GLFW_KEY_W))
                        transform.Translate.y += 0.05f;
                    if(m_Context->Window->IsKey(GLFW_KEY_S))
                        transform.Translate.y -= 0.05f;
                    
                    transform.Rotation.y -= 0.05f;                    
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