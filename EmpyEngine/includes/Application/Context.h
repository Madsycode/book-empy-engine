#pragma once
#include "Window/Window.h"
#include "Physics/Context.h"
#include "Graphics/Renderer.h"

namespace Empy
{
    // forward declaration
    struct AppInterface;    

    // application context
    struct AppContext
    {
        EMPY_INLINE AppContext()
        {
            Window = std::make_unique<AppWindow>(&Dispatcher, 1280, 720, "Empy Engine");
            Renderer = std::make_unique<GraphicsRenderer>(1280, 720);
            Physics = std::make_unique<PhysicsContext>();
            DeltaTime = 0.0;
        }

        EMPY_INLINE ~AppContext()
        {
            for(auto layer : Layers)
            {
                EMPY_DELETE(layer);
            } 
        }
        
        std::unique_ptr<GraphicsRenderer> Renderer;
        std::unique_ptr<PhysicsContext> Physics;
        std::vector<AppInterface*> Layers;
        std::unique_ptr<AppWindow> Window;
        EventDispatcher Dispatcher;
        EntityRegistry Scene;
        double DeltaTime;
    };
}