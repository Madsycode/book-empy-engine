#pragma once
#include "Interface.h"

namespace Empy
{
    struct Application : AppInterface
    {
        EMPY_INLINE Application() 
        {
            m_LayerID = TypeID<Application>();
            m_Context = new AppContext();
        }

        EMPY_INLINE ~Application() 
        {
            EMPY_DELETE(m_Context);
        }

        EMPY_INLINE void RunContext() 
        {
            while(m_Context->Window->PollEvents())
            {
                for(auto layer : m_Context->Layers)
                {
                    layer->OnUpdate();
                }              
            }
        }
    };
}