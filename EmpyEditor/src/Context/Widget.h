#pragma once
#include "Events.h"
#include "Inputs.h"

// forward decl.
struct GuiContext;

// widget interface
struct IWidget
{
    EMPY_INLINE IWidget(GuiContext*) {};
    EMPY_INLINE virtual ~IWidget() = default;
    EMPY_INLINE virtual void OnSelect(Entity) {}
    EMPY_INLINE virtual void OnShow(GuiContext*) {}
    EMPY_INLINE virtual void SetTitle(const char*) {}
};

// widget type definition
using Widget = std::unique_ptr<IWidget>;

