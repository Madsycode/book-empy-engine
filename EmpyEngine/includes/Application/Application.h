#pragma once
#include "Interface.h"

namespace Empy
{
    struct Application : AppInterface
    {
        // runs application main loop
        EMPY_INLINE void RunContext()
        {          
            // application main loop
            while(m_Context->Window->PollEvents())
            {   
                // set delta time
                UpdateDeltaTime();

                // update scene, 
                UpdateScene();

                // render scene 
                RenderScene();

                // update layers
                UpdateLayers();
            }
        }

        // destroy application context
        EMPY_INLINE ~Application() 
        {
            EMPY_DELETE(m_Context);
        }

        // creates application context
        EMPY_INLINE Application() 
        {
            // create application context
            m_LayerID = TypeID<Application>();                   
            m_Context = new AppContext();

            // register callbacks
            RegisterCallbacks();

            // start scene
            StartScene();
        }

    private:   
        // registers event callback functions
        EMPY_INLINE void RegisterCallbacks()
        {
            // set physics event callback
            m_Context->Physics->SetEventCallback([this] (auto e)
            {
                // do not report if invalid entities        
                if(!m_Context->Scene.valid(e.Entity1) || 
                !m_Context->Scene.valid(e.Entity2)) 
                { return; }

                PostTask([this, e] 
                {
                    auto entity1 = ToEntt<Entity>(e.Entity1);
                    auto entity2 = ToEntt<Entity>(e.Entity2);

                    // entity 1 call script on-collision  
                    if(entity1.template Has<ScriptComponent>())
                    {
                        entity1.template Get<ScriptComponent>().
                        Instance->OnCollision(e.Entity2);
                    }

                    // entity 2 call script on-collision                  
                    if(entity2.template Has<ScriptComponent>())
                    {
                        entity2.template Get<ScriptComponent>().
                        Instance->OnCollision(e.Entity1);
                    }
                });
            });

            // attach window resize event callback
            AttachCallback<WindowResizeEvent>([this] (auto e) 
            {
                // resire renderer frame buffer
                m_Context->Renderer->Resize(e.Width, e.Height);

                // call scripts resize function
                EnttView<Entity, ScriptComponent>([e] 
                (auto entity, auto& script) 
                {
                    if(script.Instance) 
                    { 
                        script.Instance->OnResize(e.Width, e.Height); 
                    }
                });
            });    

            // register mouse down callback
            AttachCallback<MouseDownEvent>([this] (auto e) 
            {
                // call scripts mouse down callback
                EnttView<Entity, ScriptComponent>([e] 
                (auto entity, auto& script) 
                {
                    if(script.Instance) 
                    { 
                        script.Instance->OnMouseDown(e.Button); 
                    }
                });
            });

            // register key down callback
            AttachCallback<KeyPressEvent>([this] (auto e) 
            {
                // call scripts mouse down callback
                EnttView<Entity, ScriptComponent>([e] 
                (auto entity, auto& script) 
                {
                    if(script.Instance) 
                    { 
                        script.Instance->OnKeyDown(e.Key); 
                    }
                });
            });
        }

        // computes frame delta time value
        EMPY_INLINE void UpdateDeltaTime()
        {
            static double sLastTime = glfwGetTime();
            double currentTime = glfwGetTime();
            m_Context->DeltaTime = (currentTime - sLastTime);         
            sLastTime = currentTime;
        }
       
        // creates entities with components
        EMPY_INLINE void CreateEntities()
        {      
            // load assets
            auto skyboxAsset = m_Context->Assets->AddSkybox(RandomU64(), "Resources/Textures/HDRs/Sky.hdr", 2048);
            auto robotAsset = m_Context->Assets->AddModel(RandomU64(), "Resources/Models/Walking.fbx", true);
            auto scriptAsset = m_Context->Assets->AddScript(RandomU64(), "Resources/Scripts/TestScript.lua");
            auto sphereAsset = m_Context->Assets->AddModel(RandomU64(), "Resources/Models/sphere.fbx");
            auto cubeAsset = m_Context->Assets->AddModel(RandomU64(), "Resources/Models/cube.fbx");
            auto mtlAsset = m_Context->Assets->AddMaterial(RandomU64(), "Nimrod");
            mtlAsset->Data.Albedo.x = 0.0f;
            
            // create scene camera
            auto camera = CreateEntt<Entity>();                    
            camera.Attach<InfoComponent>();
            camera.Attach<TransformComponent>().Transform.Translate.z = 20.0f;
            camera.Attach<CameraComponent>();

            // create skybox entity
            auto skybox = CreateEntt<Entity>();                    
            skybox.Attach<InfoComponent>();
            skybox.Attach<SkyboxComponent>().Skybox = skyboxAsset->UID;
            skybox.Attach<TransformComponent>();

            // create directional light
            auto light = CreateEntt<Entity>();  
            light.Attach<InfoComponent>();
            light.Attach<DirectLightComponent>().Light.Intensity = 1.0f;
            auto& td = light.Attach<TransformComponent>().Transform;
            td.Rotation = glm::vec3(0.0f, 1.0f, -1.0f);

            // create robot entity
            auto robot = CreateEntt<Entity>();
            robot.Attach<InfoComponent>();
            auto& robotMod = robot.Attach<ModelComponent>();
            //robotMod.Material = mtlAsset->UID;
            robotMod.Model = robotAsset->UID;
            auto& tr = robot.Attach<TransformComponent>().Transform;
            tr.Translate = glm::vec3(0.0f, -14.99f, -15.0f);
            tr.Scale = glm::vec3(0.1f);

            // create plane entity (ground)
            auto plane = CreateEntt<Entity>();
            plane.Attach<InfoComponent>();
            plane.Attach<RigidBodyComponent>().RigidBody.Dynamic = false;
            plane.Attach<ColliderComponent>().Collider.Type = ColliderType::BOX;
            auto& planeMod = plane.Attach<ModelComponent>();
            planeMod.Material = mtlAsset->UID;
            planeMod.Model = cubeAsset->UID;
            auto& tp = plane.Attach<TransformComponent>().Transform;
            tp.Translate = glm::vec3(0.0f, -15.0f, -50.0f);
            tp.Scale = glm::vec3(100.0f, 1.0f, 100.0f);

            // create robot entity
            for(uint32_t i = 0; i < 10; i++)
            {
                auto cube = CreateEntt<Entity>();
                cube.Attach<InfoComponent>().Name = "Entity" + std::to_string(i);
                cube.Attach<ColliderComponent>().Collider.Type = ColliderType::BOX;
                //cube.Attach<ScriptComponent>().Script = scriptAsset->UID;
                cube.Attach<RigidBodyComponent>();
                auto& modelComp = cube.Attach<ModelComponent>();
                modelComp.Material = mtlAsset->UID;
                modelComp.Model = cubeAsset->UID;
                auto& tc = cube.Attach<TransformComponent>().Transform;
                tc.Translate = glm::vec3(0.0f, 6.0f * i, -10.0f);
                tc.Scale *= 5.0f; 
            }
            // set all target models
            m_Context->Serializer->Serialize(*m_Context->Assets, "Resources/Projects/assets.yaml");
            m_Context->Serializer->Serialize(m_Context->Scene, "Resources/Projects/scene.yaml");           
        }
        
        // updates all application layers
        EMPY_INLINE void UpdateLayers()
        {
            for(auto layer : m_Context->Layers)
            {
                layer->OnUpdate();
            }    
            // show scene to screen
            m_Context->Renderer->ShowFrame();
        }      

        // updates physcis, scripts, etc.
        EMPY_INLINE void UpdateScene()
        {
            // update script instances
            EnttView<Entity, ScriptComponent>([this] (auto entity, auto& script) 
            {
                if(script.Instance)
                {
                    script.Instance->OnUpdate(m_Context->DeltaTime);
                }
            });

            // compute physics state
            m_Context->Physics->Simulate(1, m_Context->DeltaTime);      

            // update physics physics
            EnttView<Entity, RigidBodyComponent>([this] (auto entity, auto& comp) 
            { 
                auto& transform = entity.template Get<TransformComponent>().Transform;     
                auto pose = comp.RigidBody.Actor->getGlobalPose();
                glm::quat rot(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
                transform.Rotation = glm::degrees(glm::eulerAngles(rot));
                transform.Translate = PxToVec3(pose.p);
            }); 

        }

        // renders depth map, color, etc.
        EMPY_INLINE void RenderScene()
        {
            // ----------------------------- SHADWO MAP -------------------------------------

            EnttView<Entity, DirectLightComponent>([this] (auto light, auto&) 
            {
                // light direction
                auto& lightDir = light.template Get<TransformComponent>().Transform.Rotation;
               
                // begin rendering
                m_Context->Renderer->BeginShadowPass(lightDir);

                // render depth 
                EnttView<Entity, ModelComponent>([this, &lightDir] (auto entity, auto& comp) 
                {    
                    auto& transform = entity.template Get<TransformComponent>().Transform;
                    auto& model = m_Context->Assets->Get<ModelAsset>(comp.Model);
                    m_Context->Renderer->DrawDepth(model.Data, transform);                                     
                }); 

                // ffinalize frame
                m_Context->Renderer->EndShadowPass();
            });

            // ------------------------ RENDER TO FBO --------------------------------------

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

            // render models
            EnttView<Entity, ModelComponent>([this] (auto entity, auto& comp) 
            {      
                // retrieve assets
                auto& transform = entity.template Get<TransformComponent>().Transform;
                auto& material = m_Context->Assets->Get<MaterialAsset>(comp.Material);
                auto& model = m_Context->Assets->Get<ModelAsset>(comp.Model);

                // render model
                m_Context->Renderer->Animate(model.Data, m_Context->DeltaTime);
                m_Context->Renderer->Draw(model.Data, material.Data, transform); 
            });  

            // render skybox
            EnttView<Entity, SkyboxComponent>([this] (auto entity, auto& comp) 
            {      
                auto& transform = entity.template Get<TransformComponent>().Transform;
                auto& skybox = m_Context->Assets->Get<SkyboxAsset>(comp.Skybox);
                m_Context->Renderer->DrawSkybox(skybox.Data, transform);                    
            });

            // end frame                
            m_Context->Renderer->EndFrame();         
        }       
                       
        // starts physics, scripts, etc.
        EMPY_INLINE void StartScene()
        {
            // create scene entities
            CreateEntities();

            // deserialize scene 
            m_Context->Serializer->Deserialize(*m_Context->Assets, "Resources/Projects/assets.yaml");
            m_Context->Serializer->Deserialize(m_Context->Scene, "Resources/Projects/scene.yaml");

            // generate enviroment maps
            EnttView<Entity, SkyboxComponent>([this] (auto entity, auto& comp) 
            {      
                auto& skybox = m_Context->Assets->Get<SkyboxAsset>(comp.Skybox);                        
                m_Context->Renderer->InitSkybox(skybox.Data, skybox.EnvMap, skybox.Size);                
            });          

            // creates and start scripts
            EnttView<Entity, ScriptComponent>([this] (auto entity, auto& comp) 
            {
                auto& script = m_Context->Assets->Get<ScriptAsset>(comp.Script);                        
                auto name = m_Context->Scripts->LoadScript(script.Source);
                m_Context->Scripts->AttachScript(entity, name);
            });

            // create rigid bodies
            EnttView<Entity, RigidBodyComponent>([this] (auto entity, auto& comp) 
            { 
                m_Context->Physics->AddRigidBody(entity);               
            });
        }
    };
}