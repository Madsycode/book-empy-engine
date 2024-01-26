#pragma once
#include "Helpers.h"

namespace Empy
{
    struct RigidBody3D 
    {
        EMPY_INLINE RigidBody3D(const RigidBody3D&) = default;
        EMPY_INLINE RigidBody3D() = default; 
        
        PxRigidActor* Actor = nullptr;
        float Density = 1.0f;
        bool Dynamic = true;
    };

    // collider shape type
    enum ColliderType : uint8_t 
    { 
        UNKNOWN = 0, 
        CAPSULE,
        SPHERE, 
        MESH, 
        BOX
    };    

    struct Collider3D
    {
        EMPY_INLINE Collider3D(const Collider3D&) = default;
        EMPY_INLINE Collider3D() = default; 

        // collider material data
        float DynamicFriction = 0.5f;
        float StaticFriction = 0.3f;
        float Restitution = 0.4f;

        // mesh for custom shape
        PxMaterial* Material = nullptr;
        PxConvexMeshGeometry Mesh;
        PxShape* Shape = nullptr;    
        ColliderType Type;        
    };
}