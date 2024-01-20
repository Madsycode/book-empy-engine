#pragma once
#include <PxPhysicsAPI.h>
#include "Graphics/Utilities/Data.h"

using namespace physx;

namespace Empy
{
    // helper function to convert PhysX Vec3 to GLM vec3
    EMPY_INLINE glm::vec3 PxToVec3(const PxVec3& physxVec) 
    {
        return glm::vec3(physxVec.x, physxVec.y, physxVec.z);
    }

    // helper function to convert GLM vec3 to PhysX Vec3
    EMPY_INLINE PxVec3 ToPxVec3(const glm::vec3& glmVec) 
    {
        return PxVec3(glmVec.x, glmVec.y, glmVec.z);
    }
} 
