#pragma once
#include "IControl.h"

struct TransformControl : IControl<TransformComponent>
{    
    EMPY_INLINE TransformControl(GuiContext* context): IControl(context)
	{
		SetTitle(ICON_FA_LOCATION_ARROW "\tTransform");
	}

    EMPY_INLINE void OnBody(GuiContext* context, Entity& entity) 
	{
        auto& data = entity.template Get<TransformComponent>().Transform;
        InputVec3("Translate", &data.Translate);
        InputVec3("Rotation", &data.Rotation);
        InputVec3("Scale", &data.Scale);    
    }

    EMPY_INLINE void OnMenu(GuiContext* context, Entity& entity) 
	{
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) 
		{
            entity.template Get<TransformComponent>() = {};
        }        
    }
};
