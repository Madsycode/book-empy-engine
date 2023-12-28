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
            // create scene camera
            auto camera = CreateEntt<Entity>();                    
            camera.Attach<TransformComponent>().Transform.Translate.z = 2.0f;
            camera.Attach<CameraComponent>();

            // create quad entity
            auto quad = CreateEntt<Entity>();
            quad.Attach<MeshComponent>().Mesh = CreateQuad3D();
            quad.Attach<TransformComponent>();

            while(m_Context->Window->PollEvents())
            {
                // render new frame
                m_Context->Renderer->NewFrame(); 
                {  
                    // set shader camera
                    EnttView<Entity, CameraComponent>([this] (auto entity, auto& comp) 
                    {      
                        auto& transform = entity.template Get<TransformComponent>().Transform;
                        m_Context->Renderer->SetCamera(comp.Camera, transform);
                    });

                    // render models
                    EnttView<Entity, MeshComponent>([this] (auto entity, auto& comp) 
                    {      
                        auto& transform = entity.template Get<TransformComponent>().Transform;
                        m_Context->Renderer->Draw(comp.Mesh, transform);
                    });
                }
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