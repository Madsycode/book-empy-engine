#pragma once
#include "Context/Context.h"

struct ViewportWindow : IWidget
{	
	EMPY_INLINE ViewportWindow(GuiContext* context): IWidget(context)
	{
		m_Frame = (ImTextureID)context->GetSceneFrame();
	}

    EMPY_INLINE void OnShow(GuiContext* context) override
	{
        if(ImGui::Begin(ICON_FA_IMAGE "\tViewport")) 
		{			
			ImGui::BeginChild("Frame");
        	{	
				// show scene frame
				const ImVec2 size = ImGui::GetWindowContentRegionMax();
				ImGui::Image(m_Frame, size, ImVec2(0, 1), ImVec2(1, 0));

				// handle resizing
				if(m_Viewport.x != size.x || m_Viewport.y != size.y) 
				{
					int32_t width = (int32_t)size.x, height = (int32_t)size.y;
					context->PostEvent<WindowResizeEvent>(width, height);
					m_Viewport = size;
				}		

				// get imgui io
				auto& io = ImGui::GetIO();

				if(ImGui::IsWindowHovered())
				{
					// handle zoom in/out 
					if(io.MouseWheel != 0) 
					{
						float sensibility = 50;

						context->EnttView<Entity, CameraComponent>([&] (auto entity, auto& comp) 
						{
							auto& transform = entity.template Get<TransformComponent>().Transform;
							transform.Translate.z -= (io.MouseWheel * io.DeltaTime * sensibility);
						});
					} 	 

					// handle dragging
					if(ImGui::IsMouseDragging(ImGuiMouseButton_Left)) 
					{
						float sensibility = 10;

						context->EnttView<Entity, CameraComponent>([&] (auto entity, auto& comp) 
						{
							auto& transform = entity.template Get<TransformComponent>().Transform;
							transform.Rotation.x += (io.MouseDelta.y * io.DeltaTime * sensibility);
							transform.Rotation.y += (io.MouseDelta.x * io.DeltaTime * sensibility);
						});
                    }

				}
			}
			ImGui::EndChild();
        }
        ImGui::End();
    }

	EMPY_INLINE void OnSelect(Entity entity) override 
	{
						
	}
	
private:
	ImTextureID m_Frame;
	ImVec2 m_Viewport;
};