#pragma once
#include "Auxiliaries/ECS.h"

namespace Empy
{
    enum class PxEvent
    { 
        UNKNOWN = 0,
        TRIGGER, 
        CONTACT, 
        SLEEP, 
        WAKE 
    };

    struct PxPayload
    { 
        EntityID Entity1 = NENTT;
        EntityID Entity2 = NENTT;
        PxEvent Event = PxEvent::UNKNOWN;
    };

    struct ActorData
    { 
        EntityID Entt = NENTT;
    };

    using PxCallbackFunction = std::function<void(const PxPayload&)>;

    struct PxEventCallback : public PxSimulationEventCallback 
    {
        // override the onContact callback
        EMPY_INLINE void onContact(const PxContactPairHeader& header, const 
        PxContactPair* pairs, PxU32 nbPairs) override  
        {
            // collision actors
            auto actor1 = header.actors[0];
            auto actor2 = header.actors[1];

            // check if actors
            if (m_Callback && actor1 && actor2) 
            {
                PxPayload event;
                event.Event = PxEvent::CONTACT;
                event.Entity1 = *static_cast<EntityID*>(actor1->userData);
                event.Entity2 = *static_cast<EntityID*>(actor2->userData);
                m_Callback(event);                     
            }

            EMPY_DEBUG("onContact Event!");
        }
   
        // override the onTrigger callback
        EMPY_INLINE void onTrigger(PxTriggerPair* pairs, PxU32 nbPairs) override  
        {
             for (PxU32 i = 0; m_Callback && i < nbPairs; ++i) 
             {
                // Access actor pointers from the trigger pair
                const PxRigidActor* actor0 = pairs[i].triggerActor;
                const PxRigidActor* actor1 = pairs[i].otherActor;

                // Check if actors are valid before using
                if (actor0 && actor1) 
                {
                    PxPayload event;
                    event.Event = PxEvent::TRIGGER;
                    event.Entity1 = *static_cast<EntityID*>(actor1->userData);
                    event.Entity2 = *static_cast<EntityID*>(actor0->userData);
                    m_Callback(event);                     
                }
            }
            EMPY_DEBUG("onTrigger Event!");
        }
        
	    EMPY_INLINE void onAdvance(const PxRigidBody*const* bodyBuffer, 
        const PxTransform* poseBuffer, const PxU32 count) override {}

        EMPY_INLINE void onSleep(PxActor** actors, PxU32 count) 
        override { EMPY_DEBUG("onSleep Event!"); }

        EMPY_INLINE void onWake(PxActor** actors, PxU32 count) 
        override { EMPY_DEBUG("onWake Event!"); }

        EMPY_INLINE void onConstraintBreak(PxConstraintInfo* 
        constraints, PxU32 count) override {}

    private:
        PxCallbackFunction m_Callback;
        friend struct PhysicsContext;
    };
} 
