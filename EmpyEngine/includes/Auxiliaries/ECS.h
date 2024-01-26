#pragma once
#include "Assets.h"

namespace Empy
{   
    // direct light component
    struct DirectLightComponent 
    {
        EMPY_INLINE DirectLightComponent(const DirectLightComponent&) = default;
        EMPY_INLINE DirectLightComponent() = default; 
        DirectLight Light;
    };

    // point light component
    struct PointLightComponent 
    {
        EMPY_INLINE PointLightComponent(const PointLightComponent&) = default;
        EMPY_INLINE PointLightComponent() = default; 
        PointLight Light;
    };

    // point light component
    struct SpotLightComponent 
    {
        EMPY_INLINE SpotLightComponent(const SpotLightComponent&) = default;
        EMPY_INLINE SpotLightComponent() = default; 
        SpotLight Light;
    };

    // transform component
    struct TransformComponent 
    {
        EMPY_INLINE TransformComponent(const TransformComponent&) = default;
        EMPY_INLINE TransformComponent() = default; 
        Transform3D Transform;
    }; 
    
    // rigid body component
    struct RigidBodyComponent 
    {
        EMPY_INLINE RigidBodyComponent(const RigidBodyComponent&) = default;
        EMPY_INLINE RigidBodyComponent() = default; 
        RigidBody3D RigidBody;
    }; 

    // collider component
    struct ColliderComponent 
    {
        EMPY_INLINE ColliderComponent(const ColliderComponent&) = default;
        EMPY_INLINE ColliderComponent() = default; 
        Collider3D Collider;
    }; 

    // camera component
    struct CameraComponent 
    {
        EMPY_INLINE CameraComponent(const CameraComponent&) = default;
        EMPY_INLINE CameraComponent() = default; 
        Camera3D Camera; 
    }; 

    // skybox component
    struct SkyboxComponent 
    {
        EMPY_INLINE SkyboxComponent(const SkyboxComponent&) = default;
        EMPY_INLINE SkyboxComponent() = default; 
        AssetID Skybox = EMPTY_ASSET;
    };    

    // script component
    struct ScriptComponent 
    {
        EMPY_INLINE ScriptComponent(const ScriptComponent&) = default;
        EMPY_INLINE ScriptComponent() = default; 
        AssetID Script = EMPTY_ASSET;
        LuaScript Instance;
    }; 

    // model component
    struct ModelComponent 
    { 
        EMPY_INLINE ModelComponent(const ModelComponent&) = default;
        EMPY_INLINE ModelComponent() = default; 
        AssetID Material = EMPTY_ASSET; 
        AssetID Model = EMPTY_ASSET; 
    };

    // common component
    struct InfoComponent 
    { 
        EMPY_INLINE InfoComponent(const InfoComponent&) = default;
        EMPY_INLINE InfoComponent() = default; 
        AssetID Parent = EMPTY_ASSET;
        std::string Name = "Entity"; 
        AssetID UID = RandomU64();
    };

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