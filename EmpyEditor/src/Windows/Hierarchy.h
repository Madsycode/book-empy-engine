#pragma once
#include "Context/Context.h"

struct HierarchyWindow : IWidget
{	
	EMPY_INLINE HierarchyWindow(GuiContext* context): IWidget(context)
	{

	}

    EMPY_INLINE void OnShow(GuiContext* context) override
	{
        if(ImGui::Begin(ICON_FA_CUBES "\tHierarchy")) 
		{
			context->EnttView<Entity, InfoComponent>([&] (auto entity, auto& info) 
			{	
				// check if current entity is selected
				bool isSelected = (m_Selected.ID() == entity.ID());

				// makes each selectable unique
				ImGui::PushID((int32_t)entity.ID());

				// compute selectable label
				std::string label = ICON_FA_CUBE "\t" + info.Name;

				// special label with icons
				if(entity.template Has<DirectLightComponent>())
					label = ICON_FA_SUN "\t" + info.Name;
				if(entity.template Has<CameraComponent>())
					label = ICON_FA_VIDEO "\t" + info.Name;
				if(entity.template Has<RigidBodyComponent>())
					label = ICON_FA_RING "\t" + info.Name;

				// show entity selectable
				if(ImGui::Selectable(label.c_str(), &isSelected))
				{
					context->PostEvent<SelectEvent>(entity);
				}

				// w are required to pop
				ImGui::PopID();
			});
        }
        ImGui::End();
    }

	EMPY_INLINE void OnSelect(Entity entity) override 
	{
		m_Selected = entity;						
	}
	
private:
	Entity m_Selected;
};