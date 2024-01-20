#pragma once
#include "Helpers.h"

namespace Empy
{
    struct Script
    {
        // script handle constructor
        EMPY_INLINE Script(sol::table handle, const std::string& name): 
            m_Instance(handle), m_Name(name) 
        {}

        // callback for window resize event
        EMPY_INLINE void OnResize(int32_t width, int32_t height) 
        { 
            if(m_Instance.valid())
            {
                m_Instance["OnResize"](m_Instance, width, height);
            } 
        }  
        
        // callback for mouse down event
        EMPY_INLINE void OnMouseDown(int32_t button) 
        { 
            if(m_Instance.valid())
            {
                m_Instance["OnMouseDown"](m_Instance, button); 
            }
        }

        // callback for rigidbody collision 
        EMPY_INLINE void OnCollision(EntityID other) 
        { 
            if(m_Instance.valid())
            {
                m_Instance["OnCollision"](m_Instance, other);
            } 
        }

        // callback for key down event
        EMPY_INLINE void OnKeyDown(int32_t key) 
        { 
            if(m_Instance.valid())
            {
                m_Instance["OnKeyDown"](m_Instance, key); 
            } 
        }

        // callback to update script 
        EMPY_INLINE void OnUpdate(float dt) 
        { 
            if(m_Instance.valid())
            {
                m_Instance["OnUpdate"](m_Instance, dt);
            } 
        }

        // callback to destroy entity 
        EMPY_INLINE void OnDestroy() 
        { 
            if(m_Instance.valid())
            {
                m_Instance["OnDestroy"](m_Instance); 
            }
        }

        // callback sto tart script 
        EMPY_INLINE void OnStart() 
        { 
            if(m_Instance.valid())
            {
                m_Instance["OnStart"](m_Instance); 
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
            return m_Instance.valid(); 
        }

    private:
        friend struct ScriptContext;
        sol::table m_Instance;
        std::string m_Name;
    };

    using LuaScript = std::unique_ptr<Script>;
}