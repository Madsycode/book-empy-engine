#pragma once
#include "Helpers.h"

namespace Empy
{
    struct RigidBody3D 
    {
        EMPY_INLINE RigidBody3D(const RigidBody3D&) = default;
        EMPY_INLINE RigidBody3D() = default; 
        
        // body actor pointer
        PxRigidActor* Actor = nullptr;

        // body density
        float Density = 1.0f;

        // rigidbody type
        enum { 
            DYNAMIC = 0, 
            STATIC, 
        } Type;
    };

    struct Collider3D
    {
        EMPY_INLINE Collider3D(const Collider3D&) = default;
        EMPY_INLINE Collider3D() = default; 

        // collider matrial pointer
        PxMaterial* Material = nullptr;

        // collider material data
        float DynamicFriction = 0.5f;
        float StaticFriction = 0.0f;
        float Restitution = 0.1f;

        // mesh for custom shape
        PxConvexMeshGeometry Mesh;

        // collider geometry shape
        PxShape* Shape = nullptr;

        // collider shape type
        enum { 
            BOX = 0, 
            SPHERE, 
            MESH 
        } Type;        
    };
}