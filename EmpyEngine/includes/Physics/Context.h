#pragma once
#include "Utilities.h"

namespace Empy
{
    struct PhysicsContext
    {
        EMPY_INLINE PhysicsContext() 
        {
            // initialize physX SDK
            m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_AllocatorCallback, m_ErrorCallback);
            if (!m_Foundation) 
            {
                EMPY_ERROR("Error initializing PhysX m_Foundation");
                return;
            }

            // create context instance
            m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale());
            if (!m_Physics) 
            {
                EMPY_ERROR("Error initializing PhysX m_Physics");
                m_Foundation->release();
                return;
            }

            // create worker threads
            m_Dispatcher = PxDefaultCpuDispatcherCreate(2); 

            // create a scene desciption
            PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
            sceneDesc.filterShader = PxDefaultSimulationFilterShader;
            sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
            sceneDesc.cpuDispatcher = m_Dispatcher;

            // create scene instance
            m_Scene = m_Physics->createScene(sceneDesc);            
            if (!m_Scene) 
            {
                EMPY_ERROR("Error creating PhysX m_Scene");
                m_Physics->release();
                m_Foundation->release();
                return;
            }         
        }

        EMPY_INLINE ~PhysicsContext()
        {
            if (m_Scene) { m_Scene->release(); }
            if (m_Physics) { m_Physics->release(); }
            if (m_Dispatcher) { m_Dispatcher->release(); }
            if (m_Foundation) { m_Foundation->release(); }
        }

        EMPY_INLINE void Simulate(uint32_t step, float dt)
        {
            for (int i = 0; i < step; ++i) 
            {
                // simulate m_Physics for a time step
                m_Scene->simulate(dt); 
                // block until simulation is complete
                m_Scene->fetchResults(true); 
            }
        }
                
        EMPY_INLINE void AddActor(RigidBody3D& body, Transform3D& transform)
        {            
            // set friction and restitution 

            // create rigidbody transformation
            PxTransform pose(ToPxVec3(transform.Translate)); 
            glm::quat rot(transform.Rotation);
            pose.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

            // create a rigid body actor
            if(body.Type == RigidBody3D::DYNAMIC) 
            {   
                body.Actor = m_Physics->createRigidDynamic(pose);
            }
            else if(body.Type == RigidBody3D::STATIC)
            {
                body.Actor = m_Physics->createRigidStatic(pose);
            }
            else 
            {
                EMPY_ERROR("Error creating rigidbody invalid type");
                return;
            }

            // check actor
            if (!body.Actor) 
            {
                EMPY_ERROR("Error creating dynamic actor");               
                return;
            }

            // add actor to the m_Scene
            m_Scene->addActor(*body.Actor);               
        }

        EMPY_INLINE void AddActor(RigidBody3D& body, Collider3D& collider, Transform3D& transform)
        {            
            // create rigidbody transformation
            PxTransform pose(ToPxVec3(transform.Translate)); 
            glm::quat rot(transform.Rotation);
            pose.q = PxQuat(rot.x, rot.y, rot.z, rot.w);

            // create collider shape
            collider.Material = m_Physics->createMaterial(
                collider.StaticFriction, 
                collider.DynamicFriction, 
                collider.Restitution
            );
          
            if(collider.Type == Collider3D::BOX)
            {
                PxBoxGeometry box(ToPxVec3(transform.Scale/2.0f));
                collider.Shape = m_Physics->createShape(box, *collider.Material);
            }
            else if(collider.Type == Collider3D::SPHERE)
            {
                PxSphereGeometry sphere(transform.Scale.x/2.0f);
                collider.Shape = m_Physics->createShape(sphere, *collider.Material);
            }   
            else if(collider.Type == Collider3D::MESH)
            {
                collider.Shape = m_Physics->createShape(collider.Mesh, *collider.Material, true);
            }            
            else 
            {
                EMPY_ERROR("Error creating collider invalid type");
                return;
            }

            // create a rigid body actor
            if(body.Type == RigidBody3D::DYNAMIC) 
            {             
                body.Actor = PxCreateDynamic(*m_Physics, pose, *collider.Shape, body.Density);                           
            }
            else if(body.Type == RigidBody3D::STATIC)
            {
                body.Actor = PxCreateStatic(*m_Physics, pose, *collider.Shape);
            }
            else 
            {
                EMPY_ERROR("Error creating rigidbody invalid type");
                return;
            }

            // check actor
            if (!body.Actor) 
            {
                EMPY_ERROR("Error creating dynamic actor");               
                return;
            }

            // add actor to the m_Scene
            m_Scene->addActor(*body.Actor);               
        }

        EMPY_INLINE PxConvexMeshGeometry CookMesh(const MeshData<ShadedVertex>& data) 
        {          
            // px vertex container
            std::vector<PxVec3> vertices;

            // convert position attributes
            for(auto& vertex : data.Vertices)
            {
                vertices.push_back(ToPxVec3(vertex.Position));
            }

            PxConvexMeshDesc meshDesc;
            // vertices
            meshDesc.points.data = vertices.data();
            meshDesc.points.stride = sizeof(PxVec3);
            meshDesc.points.count = vertices.size();
            // indices 
            meshDesc.indices.data = data.Indices.data();
            meshDesc.indices.count = data.Indices.size();
            // flags
            meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

            // cooking the mesh
            PxTolerancesScale default;
            PxCookingParams cookingParams = PxCookingParams(default);    
            PxCooking* cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, cookingParams);
            PxConvexMeshCookingResult::Enum result;            
            PxConvexMesh* convexMesh = cooking->createConvexMesh(meshDesc, 
            m_Physics->getPhysicsInsertionCallback(), &result);
            PxConvexMeshGeometry convexMeshGeometry(convexMesh);

            cooking->release();
            return convexMeshGeometry;
        }

    private:
        PxDefaultErrorCallback m_ErrorCallback;
        PxDefaultAllocator m_AllocatorCallback;
        PxDefaultCpuDispatcher* m_Dispatcher;
        PxFoundation* m_Foundation;
        PxPhysics* m_Physics;
        PxScene* m_Scene;
    };
}