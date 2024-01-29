#pragma once
#include "IControl.h"

struct DirectLightControl : IControl<DirectLightComponent>
{    
    EMPY_INLINE DirectLightControl(GuiContext* context): IControl(context)
	{
		SetTitle(ICON_FA_SUN "\tDirect Light");
	}

    EMPY_INLINE void OnBody(GuiContext* context, Entity& entity) 
	{
        auto& data = entity.template Get<DirectLightComponent>().Light;
        InputFloat("Intensity", &data.Intensity);
        InputColor("Radiance", &data.Radiance.x);
        InputFloat("Bias", &data.ShadowBias);    
    }

    EMPY_INLINE void OnMenu(GuiContext* context, Entity& entity) 
	{
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) 
		{
            entity.template Get<DirectLightComponent>() = {};
        }        
    }
};
