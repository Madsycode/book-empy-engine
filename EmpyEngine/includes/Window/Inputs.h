#pragma once
#include "Common/Event.h"
#include <GLFW/glfw3.h>

namespace Empy
{
    struct WindowInputs
    {
        std::bitset<GLFW_MOUSE_BUTTON_LAST> Mouse;
        std::bitset<GLFW_KEY_LAST> Keys;
        double MouseX = 0.0;
        double MouseY = 0.0; 
    };
}