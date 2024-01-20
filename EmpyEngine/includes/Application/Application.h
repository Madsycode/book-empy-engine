#pragma once
#include "Interface.h"

namespace Empy
{
    struct Application : AppInterface
    {
        // creates application context
        EMPY_INLINE Application() 
        {
            m_LayerID = TypeID<Application>();                   
            m_Context = new AppContext();

            // register event callbacks
            RegisterEventCallbacks();
            // create scene entities
            CreateSceneEntities();
            // create physics actors
            CreatePhysicsActors();
            // create environm. maps
            CreateSkyboxEnvMaps();
            // start script instances
            StartScriptInstances();
        }

        // destroy application context
        EMPY_INLINE ~Application() 
        {
            // release physics actors
            DestroyPhysicsActors();
            EMPY_DELETE(m_Context);
        }

        // runs application main loop
        EMPY_INLINE void RunContext()
        {          
            // application main loop
            while(m_Context->Window->PollEvents())
            {   
                // compute and update delta time value
                ComputeFrameDeltaTime();

                // call update func script instances
                UpdateScriptInstances();

                // run and fetch physics simulation
                RunPhysicsSimulation();

                // render scene shadow map
                RenderSceneDepthMap();

                // render scene to buffer
                RenderSceneToFBO();                          

                // update all layers
                UpdateAppLayers();
            }
        }

    private:     
        EMPY_INLINE void RegisterEventCallbacks()
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

        // call script instances update funcs 
        EMPY_INLINE void UpdateScriptInstances()
        {
            EnttView<Entity, ScriptComponent>([this] 
            (auto entity, auto& script) 
            {
                if(script.Instance)
                {
                    script.Instance->OnUpdate(m_Context->DeltaTime);
                }
            });
        }

        // call script instances start funcs 
        EMPY_INLINE void StartScriptInstances()
        {
            EnttView<Entity, ScriptComponent>([this] 
            (auto entity, auto& script) 
            {
                if(script.Instance)
                {
                    script.Instance->OnStart();
                }
            });
        }

        // computes frame delta time value
        EMPY_INLINE void ComputeFrameDeltaTime()
        {
            static double sLastTime = glfwGetTime();
            double currentTime = glfwGetTime();
            m_Context->DeltaTime = (currentTime - sLastTime);         
            sLastTime = currentTime;
        }

        // runs physics and fetch physics
        EMPY_INLINE void RunPhysicsSimulation()
        {
            // compute physx
            m_Context->Physics->Simulate(1, m_Context->DeltaTime);      

            // start physics
            EnttView<Entity, RigidBodyComponent>([this] (auto entity, auto& comp) 
            { 
                auto& transform = entity.template Get<TransformComponent>().Transform;     
                auto& pose = comp.RigidBody.Actor->getGlobalPose();
                glm::quat rot(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
                transform.Rotation = glm::degrees(glm::eulerAngles(rot));
                transform.Translate = PxToVec3(pose.p);
            }); 
        }

        // destroys all actors and colliders
        EMPY_INLINE void DestroyPhysicsActors()
        {
            EnttView<Entity, RigidBodyComponent>([this] (auto entity, auto& comp) 
            { 
                if(entity.template Has<ColliderComponent>())
                {
                    auto& collider = entity.template Get<ColliderComponent>().Collider;     
                    collider.Material->release();
                    collider.Shape->release();
                }

                // destroy actor user data                
                EntityID* owner = static_cast<EntityID*>(comp.RigidBody.Actor->userData);
                EMPY_DELETE(owner);

                // destroy actor instance
                comp.RigidBody.Actor->release();
            });
        }

        // creates maps for ambient lighting
        EMPY_INLINE void CreateSkyboxEnvMaps()
        {
            // load environment map
            auto skymap = std::make_shared<Texture2D>("Resources/Textures/HDRs/Sky.hdr", true, true);

            // generate enviroment maps
            EnttView<Entity, SkyboxComponent>([this, &skymap] (auto entity, auto& comp) {      
                m_Context->Renderer->InitSkybox(comp.Sky, skymap, 2048);
            });
        }

        // rendesr depth values to shadow map
        EMPY_INLINE void RenderSceneDepthMap()
        {        
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
                    m_Context->Renderer->DrawDepth(comp.Model, transform);                     
                }); 

                // ffinalize frame
                m_Context->Renderer->EndShadowPass();
            });
        }

        // creates all actors and colliders
        EMPY_INLINE void CreatePhysicsActors()
        {
            EnttView<Entity, RigidBodyComponent>([this] 
            (auto entity, auto& comp) 
            { 
                m_Context->Physics->AddRigidBody(entity);               
            });
        }

        // creates entities with components
        EMPY_INLINE void CreateSceneEntities()
        {
             // load models
            auto walking = std::make_shared<SkeletalModel>("Resources/Models/Walking.fbx");
            auto sphereModel = std::make_shared<StaticModel>("Resources/Models/sphere.fbx");
            auto cubeModel = std::make_shared<StaticModel>("Resources/Models/cube.fbx");
            
            // create scene camera
            auto camera = CreateEntt<Entity>();                    
            camera.Attach<TransformComponent>().Transform.Translate.z = 20.0f;
            camera.Attach<CameraComponent>();

            // create skybox entity
            auto skybox = CreateEntt<Entity>();                    
            skybox.Attach<TransformComponent>();
            skybox.Attach<SkyboxComponent>();

            // create directional light
            auto light = CreateEntt<Entity>();                    
            light.Attach<DirectLightComponent>().Light.Intensity = 1.0f;
            auto& td = light.Attach<TransformComponent>().Transform;
            td.Rotation = glm::vec3(0.0f, 1.0f, -1.0f);

            // create robot entity
            auto robot = CreateEntt<Entity>();
            robot.Attach<ModelComponent>().Model = walking;
            robot.Attach<AnimatorComponent>().Animator = walking->GetAnimator();
            auto& tr = robot.Attach<TransformComponent>().Transform;
            tr.Translate = glm::vec3(0.0f, -14.99f, -15.0f);
            tr.Scale = glm::vec3(0.1f);

            auto scriptName = m_Context->Scripts->
            LoadScript("Resources/Scripts/TestScript.lua");

            // create plane entity (ground)
            auto plane = CreateEntt<Entity>();
            plane.Attach<RigidBodyComponent>().RigidBody.Type = RigidBody3D::STATIC;
            plane.Attach<ColliderComponent>().Collider.Type = Collider3D::BOX;
            plane.Attach<ModelComponent>().Model = cubeModel;
            auto& tp = plane.Attach<TransformComponent>().Transform;
            tp.Translate = glm::vec3(0.0f, -15.0f, -50.0f);
            tp.Scale = glm::vec3(100.0f, 1.0f, 100.0f);

            // create robot entity
            for(int i = 0; i < 5; i++)
            {
                auto cube = CreateEntt<Entity>();
                cube.Attach<ModelComponent>().Model = cubeModel;
                m_Context->Scripts->AttachScript(cube, scriptName); 
                cube.Attach<ColliderComponent>().Collider.Type = Collider3D::BOX;
                cube.Attach<RigidBodyComponent>().RigidBody.Type = RigidBody3D::DYNAMIC;

                auto& tc = cube.Attach<TransformComponent>().Transform;
                tc.Translate = glm::vec3(0.0f, 6.0f *i, -10.0f);
                tc.Scale *= 5.0f; 
            }
        }
               
        // renders scene to the frame buffer
        EMPY_INLINE void RenderSceneToFBO()
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

            // render models
            EnttView<Entity, ModelComponent>([this] (auto entity, auto& comp) 
            {      
                // compute key frames
                if(entity.template Has<AnimatorComponent>())
                {
                    auto& animator = entity.template Get<AnimatorComponent>().Animator;
                    auto& transforms = animator->Animate(m_Context->DeltaTime);
                    m_Context->Renderer->SetJoints(transforms);
                }

                auto& transform = entity.template Get<TransformComponent>().Transform;
                m_Context->Renderer->Draw(comp.Model, comp.Material, transform);    
            });  

            // render skybox
            EnttView<Entity, SkyboxComponent>([this] (auto entity, auto& comp) 
            {      
                auto& transform = entity.template Get<TransformComponent>().Transform;
                m_Context->Renderer->DrawSkybox(comp.Sky, transform);                    
            });

            // end frame                
            m_Context->Renderer->EndFrame();         
        }       

        // updates all application layers
        EMPY_INLINE void UpdateAppLayers()
        {
            for(auto layer : m_Context->Layers)
            {
                layer->OnUpdate();
            }    

            // show scene to screen
            m_Context->Renderer->ShowFrame();
        }               
    };
}