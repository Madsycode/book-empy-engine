#pragma once
#include "Context/Context.h"

struct MenuBarWindow : IWidget 
{
	EMPY_INLINE MenuBarWindow(GuiContext* context): IWidget(context)
	{

	}

    EMPY_INLINE void OnShow(GuiContext* context) override
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));           

        if (ImGui::BeginMenuBar()) 
		{
            if (ImGui::BeginMenu("File")) 
			{
                if (ImGui::MenuItem(ICON_FA_FILE " Open Project", "Ctrl+O")) {}
                if (ImGui::MenuItem(ICON_FA_STORE " Save Scene", "Ctrl+S")) {}
                if (ImGui::MenuItem(ICON_FA_DOOR_CLOSED " Exit", "Alt+F4")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene")) 
			{
                if (ImGui::MenuItem(ICON_FA_FORWARD " Add Entity")) 
				{
					auto entity = context->CreateEntt<Entity>();
					entity.template Attach<TransformComponent>();
					entity.template Attach<InfoComponent>().Name = "New Entity";
				}

                if (ImGui::MenuItem(ICON_FA_FORWARD " Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem(ICON_FA_BACKWARD " Redo", "CTRL+Y")) {}
                ImGui::Separator();
                if (ImGui::MenuItem(ICON_FA_SCISSORS " Cut", "CTRL+X")) {}
                if (ImGui::MenuItem(ICON_FA_CLONE " Copy", "CTRL+C")) {}
                if (ImGui::MenuItem(ICON_FA_PASTE " Paste", "CTRL+V")) {}
                if (ImGui::MenuItem(ICON_FA_TRASH " Delete", "Delete")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Settings")) 
			{
                if (ImGui::MenuItem(ICON_FA_PALETTE " Theme")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Extra")) 
			{
                if (ImGui::MenuItem(ICON_FA_INFO " Help", "Ctrl+H")) {}
                if (ImGui::MenuItem(ICON_FA_QUESTION " About")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::PopStyleVar();    
    }
};