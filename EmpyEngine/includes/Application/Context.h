#pragma once
#include "Window/Window.h"

namespace Empy
{
    // foward declaration
    struct AppInterface;    

    // application context
    struct AppContext
    {
        EMPY_INLINE AppContext()
        {
            Window = std::make_unique<AppWindow>(&Dispatcher, 1280, 720, "Empy Engine");
        }

        EMPY_INLINE ~AppContext()
        {
            for(auto& layer : Layers)
            {
                EMPY_DELETE(layer);
            } 
        }
        
        std::vector<AppInterface*> Layers;
        std::unique_ptr<AppWindow> Window;
        EventDispatcher Dispatcher;
    };
}