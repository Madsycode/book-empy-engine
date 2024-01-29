#pragma once
#include "../Context/Context.h"

template <typename Component> 
struct IControl : public IWidget
{
    EMPY_INLINE virtual ~IControl() = default;

    EMPY_INLINE IControl(GuiContext* context): 
        IWidget(context)
	{}

    EMPY_INLINE void OnSelect(Entity entity) override
	{
		m_Entity = entity;
	}

	EMPY_INLINE void SetTitle(const char* title) override
	{
		m_Title = title;
	}

    EMPY_INLINE void OnShow(GuiContext* context) override
	{
		if(!m_Entity.template Has<Component>()) { return; }

        ImGui::PushID(m_Title); 
        static const auto flags = ImGuiTreeNodeFlags_DefaultOpen | 
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | 
        ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 4.0f));       
        bool isCollapsed = ImGui::CollapsingHeader(m_Title, flags);          
        ImGui::PopStyleVar();

		// activate small font
        USE_SMALL_FONT();

        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);                    
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30.0f);                    

        if(InputButton(ICON_FA_GEAR)) 
        {
            ImGui::OpenPopup(m_Title);
        }           
        
        // Popup
        if (ImGui::BeginPopup(m_Title)) 
        {	
            ImGui::Text(m_Title);
            ImGui::Separator();
            OnMenu(context, m_Entity);
            ImGui::EndPopup();
        }   

        // Content
        if(isCollapsed) 
		{
            ImGui::Spacing();
            ImGui::Indent(5.0f);
            OnBody(context, m_Entity);
            ImGui::Spacing();
            ImGui::Unindent(5.0f);
        }

        ImGui::PopID();

		// switch back to regular font
        USE_REGULAR_FONT();
    };
		
protected:
    EMPY_INLINE void virtual OnMenu(GuiContext*, Entity&) {}
    EMPY_INLINE void virtual OnBody(GuiContext*, Entity&) {}

private:
    const char* m_Title;
	Entity m_Entity;
};
