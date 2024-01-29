#pragma once
#include "IControl.h"

struct CameraControl : IControl<CameraComponent>
{    
    EMPY_INLINE CameraControl(GuiContext* context): IControl(context)
	{
		SetTitle(ICON_FA_VIDEO "\tCamera");
	}

    EMPY_INLINE void OnBody(GuiContext* context, Entity& entity) 
	{
        auto& data = entity.template Get<CameraComponent>().Camera;
        InputFloat("Near", &data.NearPlane);
        InputFloat("Far", &data.FarPlane);
        InputFloat("FOV", &data.FOV);    
    }

    EMPY_INLINE void OnMenu(GuiContext* context, Entity& entity) 
	{
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) 
		{
            entity.template Get<CameraComponent>() = {};
        }        
    }
};
