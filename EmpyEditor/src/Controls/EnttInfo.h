#pragma once
#include "IControl.h"

struct InfoControl : IControl<InfoComponent>
{    
    EMPY_INLINE InfoControl(GuiContext* context): IControl(context)
	{
		SetTitle(ICON_FA_INFO "\tEntity");
	}

    EMPY_INLINE void OnBody(GuiContext* context, Entity& entity) 
	{
        auto& data = entity.template Get<InfoComponent>();
        InputText("Name", data.Name.data(), "Untitled", 64);
    }

    EMPY_INLINE void OnMenu(GuiContext* context, Entity& entity) 
	{
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) 
		{
            entity.template Get<CameraComponent>() = {};
        }        
    }
};
