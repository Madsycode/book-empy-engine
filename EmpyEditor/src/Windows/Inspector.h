#pragma once
#include "Controls/Camera.h"
#include "Controls/EnttInfo.h"
#include "Controls/Transform.h"
#include "Controls/DirectLight.h"

struct InspectorWindow : IWidget
{	
	EMPY_INLINE InspectorWindow(GuiContext* context): IWidget(context)
	{
		m_Widgets.push_back(context->CreateWidget<InfoControl>());		
		m_Widgets.push_back(context->CreateWidget<TransformControl>());		
		m_Widgets.push_back(context->CreateWidget<CameraControl>());		
		m_Widgets.push_back(context->CreateWidget<DirectLightControl>());		
	}

    EMPY_INLINE void OnShow(GuiContext* context) override
	{
        if(ImGui::Begin(ICON_FA_CIRCLE_INFO "\tInspector")) 
		{
			for(auto& widget : m_Widgets)			
				widget->OnShow(context);			
        }
        ImGui::End();
    }

	EMPY_INLINE void OnSelect(Entity entity) override 
	{
		for(auto& widget : m_Widgets)			
			widget->OnSelect(entity);						
	}
	
private:
	std::vector<Widget> m_Widgets;
};