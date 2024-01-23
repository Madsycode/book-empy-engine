#pragma once
#include "Helpers.h"

namespace Empy
{
    struct Script
    {
        // script handle constructor
        EMPY_INLINE Script(sol::table handle, const std::string& name): 
            m_Handle(handle), m_Name(name) 
        {}

        // callback for window resize event
        EMPY_INLINE void OnResize(int32_t width, int32_t height) 
        { 
            if(m_Handle.valid())
            {
                m_Handle["OnResize"](m_Handle, width, height);
            } 
        }  
        
        // callback for mouse down event
        EMPY_INLINE void OnMouseDown(int32_t button) 
        { 
            if(m_Handle.valid())
            {
                m_Handle["OnMouseDown"](m_Handle, button); 
            }
        }

        // callback for rigidbody collision 
        EMPY_INLINE void OnCollision(EntityID other) 
        { 
            if(m_Handle.valid())
            {
                m_Handle["OnCollision"](m_Handle, other);
            } 
        }

        // callback for key down event
        EMPY_INLINE void OnKeyDown(int32_t key) 
        { 
            if(m_Handle.valid())
            {
                m_Handle["OnKeyDown"](m_Handle, key); 
            } 
        }

        // callback to update script 
        EMPY_INLINE void OnUpdate(float dt) 
        { 
            if(m_Handle.valid())
            {
                m_Handle["OnUpdate"](m_Handle, dt);
            } 
        }

        // callback to destroy entity 
        EMPY_INLINE void OnDestroy() 
        { 
            if(m_Handle.valid())
            {
                m_Handle["OnDestroy"](m_Handle); 
            }
        }

        // callback sto tart script 
        EMPY_INLINE void OnStart() 
        { 
            if(m_Handle.valid())
            {
                m_Handle["OnStart"](m_Handle); 
            }
        }

        // returns script name
        EMPY_INLINE const std::string& Name() 
        { 
            return m_Name; 
        }

        // checks if valid
        EMPY_INLINE bool Valid() 
        { 
            return m_Handle.valid(); 
        }

    private:
        friend struct ScriptContext;
        sol::table m_Handle;
        std::string m_Name;
    };

    using LuaScript = std::unique_ptr<Script>;
}