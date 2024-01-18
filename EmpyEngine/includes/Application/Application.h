#pragma once
#include "Interface.h"

namespace Empy
{
    struct Application : AppInterface
    {
        // creates application context
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

        // destroy application context
        EMPY_INLINE ~Application() 
        {
            EMPY_DELETE(m_Context);
        }

        // runs application main loop
        EMPY_INLINE void RunContext()
        {
            // create scene entities
            CreateSceneEntities();

            // create physics actors
            CreatePhysicsActors();

            // create environm. maps
            CreateSkyboxEnvMaps();

            // application main loop
            while(m_Context->Window->PollEvents())
            {
                // run and fetch physics simulation
                UpdatePhysics(1.0f/60.0f);

                // render scene shadow map
                RenderSceneDepthMap();

                // render scene to buffer
                RenderSceneToFBO();                          

                // update all layers
                UpdateAppLayers();

                // show scene to screen
                m_Context->Renderer->ShowFrame();
            }

            // destroy physics actors
            DestroyPhysicsActors();
        }

    private:               
        // runs physics simulation and fetch
        EMPY_INLINE void UpdatePhysics(float dt)
        {
            // compute physx
            m_Context->Physics->Simulate(1, dt);      

            // start physics
            EnttView<Entity, RigidBodyComponent>([this] (auto entity, auto& comp) 
            { 
                auto& transform = entity.Get<TransformComponent>().Transform;     
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
                comp.RigidBody.Actor->release();
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
            EnttView<Entity, RigidBodyComponent>([this] (auto entity, auto& comp) 
            { 
                auto& transform = entity.template Get<TransformComponent>().Transform;     
                if(entity.template Has<ColliderComponent>())
                {
                    auto& collider = entity.template Get<ColliderComponent>().Collider;     
                    m_Context->Physics->AddActor(comp.RigidBody, collider, transform);
                }
                else
                {
                    m_Context->Physics->AddActor(comp.RigidBody, transform);
                }
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

            // create directional light
            auto light = CreateEntt<Entity>();                    
            light.Attach<DirectLightComponent>().Light.Intensity = 1.0f;
            auto& td = light.Attach<TransformComponent>().Transform;
            td.Rotation = glm::vec3(0.0f, 1.0f, -1.0f);

            // create skybox entity
            auto skybox = CreateEntt<Entity>();                    
            skybox.Attach<TransformComponent>();
            skybox.Attach<SkyboxComponent>();

            // create robot entity
            auto robot = CreateEntt<Entity>();
            robot.Attach<ModelComponent>().Model = walking;
            auto& tr = robot.Attach<TransformComponent>().Transform;
            tr.Translate = glm::vec3(0.0f, -10.0f, -10.0f);
            tr.Scale = glm::vec3(0.1f);
            robot.Attach<AnimatorComponent>().Animator = walking->GetAnimator();

            // create plane entity (ground)
            auto plane = CreateEntt<Entity>();
            plane.Attach<RigidBodyComponent>().RigidBody.Type = RigidBody3D::STATIC;
            plane.Attach<ColliderComponent>().Collider.Type = Collider3D::BOX;
            plane.Attach<ModelComponent>().Model = cubeModel;
            auto& tp = plane.Attach<TransformComponent>().Transform;
            tp.Scale = glm::vec3(100.0f, 1.0f, 100.0f);
            tp.Translate.z = -50.0f;
            tp.Translate.y = -15.0f;

            // create sphere rigid bodies
            for(uint32_t i = 0; i < 100; i++)
            {
                Entity sphere = CreateEntt<Entity>();
                sphere.Attach<RigidBodyComponent>().RigidBody.Type = RigidBody3D::DYNAMIC;
                sphere.Attach<ColliderComponent>().Collider.Type = Collider3D::SPHERE;
                sphere.Attach<ModelComponent>().Model = sphereModel;
                auto& ts = sphere.Attach<TransformComponent>();
                ts.Transform.Translate.y = i*10.0f;
                ts.Transform.Translate.z = -10.0f;
                ts.Transform.Translate.x = 3.0f;
                ts.Transform.Scale *= 5.0f;
            }

            // create cube rigid bodies
            for(uint32_t i = 0; i < 100; i++)
            {
                Entity cube = CreateEntt<Entity>();
                cube.Attach<RigidBodyComponent>().RigidBody.Type = RigidBody3D::DYNAMIC;
                cube.Attach<ColliderComponent>().Collider.Type = Collider3D::BOX;
                cube.Attach<ModelComponent>().Model = cubeModel;
                auto& tc = cube.Attach<TransformComponent>();
                tc.Transform.Translate.y = i*10.0f;
                tc.Transform.Translate.z = -15.0f;
                tc.Transform.Rotation.x = i*10.0f;
                tc.Transform.Scale *= 5.0f;
            }
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
                    auto& transforms = animator->Animate(0.016666f);
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
        }
    };
}