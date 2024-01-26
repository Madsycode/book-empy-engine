#pragma once
#include "Scripts/Context.h"
#include "Physics/Context.h"
#include "Graphics/Renderer.h"
#include "Auxiliaries/Serializer.h"

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
            Scripts = std::make_unique<ScriptContext>(&Scene, Window.get());
            Renderer = std::make_unique<GraphicsRenderer>(1280, 720);
            Serializer = std::make_unique<DataSerializer>();
            Physics = std::make_unique<PhysicsContext>();
            Assets = std::make_unique<AssetRegistry>();
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
        std::unique_ptr<DataSerializer> Serializer;
        std::unique_ptr<PhysicsContext> Physics;
        std::unique_ptr<ScriptContext> Scripts;
        std::unique_ptr<AssetRegistry> Assets;
        std::vector<AppInterface*> Layers;
        std::unique_ptr<AppWindow> Window;
        EventDispatcher Dispatcher;
        EntityRegistry Scene;
        double DeltaTime;
    };
}