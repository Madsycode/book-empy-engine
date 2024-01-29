#pragma once
#include "Helpers.h"

struct SelectEvent
{
    EMPY_INLINE SelectEvent(EntityID entity): 
        EnttID(entity)
    {}    
    EntityID EnttID;
};