#pragma once
#include <entt/entt.hpp>
#include "Graphics/Utilities/Data.h"

namespace Empy
{
    // typedefs
    using EntityID = entt::entity;
    using EntityRegistry = entt::registry;
    constexpr EntityID NENTT = entt::null;

    // ++

    // point light component
    struct PointLightComponent 
    {
        EMPY_INLINE PointLightComponent(const PointLightComponent&) = default;
        EMPY_INLINE PointLightComponent() = default; 
        PointLight Light;
    };

    // transform component
    struct TransformComponent 
    {
        EMPY_INLINE TransformComponent(const TransformComponent&) = default;
        EMPY_INLINE TransformComponent() = default; 
        Transform3D Transform;
    }; 

    // camera component
    struct CameraComponent 
    {
        EMPY_INLINE CameraComponent(const CameraComponent&) = default;
        EMPY_INLINE CameraComponent() = default; 
        Camera3D Camera; 
    }; 

    // common component
    struct EnttComponent 
    { 
        EMPY_INLINE EnttComponent(const EnttComponent&) = default;
        EMPY_INLINE EnttComponent() = default; 
        std::string Name = "Untitled"; 
    };

    // model component
    struct ModelComponent 
    { 
        EMPY_INLINE ModelComponent(const ModelComponent&) = default;
        EMPY_INLINE ModelComponent() = default; 
        Model3D Model; 
    };

    // mesh component
    struct MeshComponent 
    { 
        EMPY_INLINE MeshComponent(const MeshComponent&) = default;
        EMPY_INLINE MeshComponent() = default; 
        Mesh3D Mesh; 
    };
    
    // ++

    // base entity
    struct Entity 
    {
        EMPY_INLINE Entity(EntityRegistry* registry, EntityID entity): 
            m_Registry(registry), m_EnttID(entity) 
        {}
        
        EMPY_INLINE Entity(EntityRegistry* registry): 
        m_Registry(registry) 
        {
            m_EnttID = m_Registry->create();		 			
        }

        EMPY_INLINE virtual ~Entity() = default;
        EMPY_INLINE Entity() = default;		

        EMPY_INLINE operator EntityID ()  
        { 
            return m_EnttID; 
        }

        EMPY_INLINE operator bool()  
        { 
            return m_Registry != nullptr && 
            m_Registry->valid(m_EnttID);
        }       

        EMPY_INLINE EntityID ID() 
        { 
            return m_EnttID; 
        }
        
        // ++

        template<typename T, typename... Args>
        EMPY_INLINE T& Attach(Args&&... args) 
        {
            return m_Registry->get_or_emplace<T>
            (m_EnttID, std::forward<Args>(args)...);
        }

        template<typename T>
        EMPY_INLINE void Detach() 
        { 
            m_Registry->remove<T>(m_EnttID); 
        }

        EMPY_INLINE void Destroy() 
        {
            if(m_Registry) 
            { 
                m_Registry->destroy(m_EnttID); 
            }
        }

        template<typename T>
        EMPY_INLINE bool Has()  
        { 
            return m_Registry != nullptr && 
            m_Registry->all_of<T>(m_EnttID); 
        }

        template<typename T>
        EMPY_INLINE T& Get() 
        { 
            return m_Registry->get<T>(m_EnttID); 
        }

    protected:
        EntityRegistry* m_Registry = nullptr;
        EntityID m_EnttID = NENTT;
    };
}