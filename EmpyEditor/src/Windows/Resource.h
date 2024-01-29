#pragma once
#include "Context/Context.h"

struct ResourceWindow : IWidget
{	
	EMPY_INLINE ResourceWindow(GuiContext* context): IWidget(context)
	{
		m_IconImage.Load("Resources/Icons/asset.png");
		m_Icon = (ImTextureID)m_IconImage.ID();
	}

    EMPY_INLINE void OnShow(GuiContext* context) override
	{
        if(ImGui::Begin(ICON_FA_FOLDER_OPEN "\tResources")) 
		{
			int nbrColumn = (ImGui::GetContentRegionAvail().x/ASSET_SIZE) + 1;			
			int columnCounter = 1;
			int rowCounter = 1;

			context->AssetView([&] (auto* asset) 
			{	
				if(columnCounter++ <= rowCounter * nbrColumn)				
					ImGui::SameLine();				
				else				
					rowCounter++;				

				// show asset icon
				bool iSClicked = ImGui::ImageButtonEx(asset->UID, 
				m_Icon, ImVec2(ASSET_SIZE, ASSET_SIZE), ImVec2(0, 1), 
				ImVec2(1, 0), ImVec4(0, 0, 0, 1), ImVec4(1, 1, 1, 1));			
			});
        }
        ImGui::End();
    }
	
private:
	Texture2D m_IconImage;
	ImTextureID m_Icon;
	AssetID m_Selected;
};