#pragma once
#include "Inputs.h"

namespace Empy
{
    // AppWindow Events

    struct WindowMaximizeEvent {};

    struct WindowIconifyEvent {};

    struct WindowRestoreEvent {};

    struct WindowCloseEvent {};

    struct WindowResizeEvent 
    { 
        EMPY_INLINE WindowResizeEvent(int32_t w, int32_t h): 
            Width(w), Height(h) 
        {}
        int32_t Height = 0;
        int32_t Width = 0;
    };

    // Key Events

    struct KeyReleaseEvent 
    {
        EMPY_INLINE KeyReleaseEvent(int32_t key): 
            Key(key) 
        {} 
        int32_t Key = -1;
    };

    struct KeyPressEvent 
    {
        EMPY_INLINE KeyPressEvent(int32_t key): 
            Key(key) 
        {} 
        int32_t Key = -1;
    };

    struct KeyRepeatEvent 
    {
        EMPY_INLINE KeyRepeatEvent(int32_t key): 
            Key(key) 
        {} 
        int32_t Key = -1;
    };

    // Mouse Events

    struct MouseReleaseEvent 
    {
        EMPY_INLINE MouseReleaseEvent(int32_t b): 
            Button(b) 
        {}
        int32_t Button = -1;
    };

    struct MouseDownEvent 
    {
        EMPY_INLINE MouseDownEvent(int32_t b): 
            Button(b) 
        {}
        int32_t Button = -1;
    };    

    struct MouseDragEvent 
    {
        EMPY_INLINE MouseDragEvent(double dx, double dy): 
            DeltaX(dx), DeltaY(dy) 
        {}
        double DeltaX = 0.0, DeltaY = 0.0;
    };

    struct MouseMotionEvent 
    {
        EMPY_INLINE MouseMotionEvent(double x, double y): 
            TaregtX(x), TargetY(y) 
        {}
        double TaregtX = 0.0, TargetY = 0.0;
    };

    struct MouseWheelEvent 
    {
        EMPY_INLINE MouseWheelEvent(double sx, double sy): 
            ScrollX(sx), ScrollY(sy) 
        {}
        double ScrollX = 0.0, ScrollY = 0.0;        
    }; 
}