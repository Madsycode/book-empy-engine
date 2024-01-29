#include "Helpers.h"

// end input field
EMPY_INLINE void EndInput()
{
    ImGui::EndColumns();
    ImGui::PopID();    
}

// starts input field
EMPY_INLINE void BeginInput(const char* label)
{
    ImGui::PushID(label);
    ImGui::Columns(2, NULL, false);
    ImGui::SetColumnWidth(0, LABEL_SPACING);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", label);
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);
}

// --------------------------------------------------

// shows bool input field
EMPY_INLINE bool InputBool(const char* label, bool* value) 
{
    BeginInput(label);        
    bool hasChanged = ImGui::Checkbox("##", value);
    EndInput();
    return hasChanged;
}

// shows float input field
EMPY_INLINE bool InputFloat(const char* label, float* value) 
{
    BeginInput(label);        
    bool hasChanged = ImGui::InputFloat("##", value);        
    EndInput();
    return hasChanged;
}

// shows vec3 input field
EMPY_INLINE bool InputVec3(const char* label, glm::vec3* value) 
{
    BeginInput(label);        
    bool hasChanged = ImGui::InputFloat3("##", &value->x);        
    EndInput();
    return hasChanged;
}

// shows bool input field
EMPY_INLINE bool InputColor(const char* label, float* value) 
{
    BeginInput(label);        
    bool hasChanged = ImGui::ColorEdit3("##", value);
    EndInput();
    return hasChanged;
}

// shows button
EMPY_INLINE bool InputButton(const char* label, ImVec2& size = ImVec2(0,0)) 
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.0f, 1.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1.0f);
    bool clicked = ImGui::ButtonEx(label, size);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    return clicked;
}

// shows combo input field
EMPY_INLINE bool InputCombo(const char* label, const std::vector<const char*>& combos) 
{
    BeginInput(label);    
    bool hasChanged = false;
    static char* preview = "Select";

    if (ImGui::BeginCombo("##", preview)) 
    {
        for (auto i = 0; i < combos.size(); i++) 
        {
            if (ImGui::Selectable(combos[i], !strcmp(combos[i], preview))) 
            {
                strcpy(preview, combos[i]);
                hasChanged = true;
            }
        }
        ImGui::EndCombo();
    }
    EndInput();

    return hasChanged;
}

// shows text input field
EMPY_INLINE bool InputText(const char* label, char* value, const char* hint = nullptr, size_t size = 32) 
{
    BeginInput(label);    
    bool hasChanged = ImGui::InputTextEx("##", hint, value, 
    size, ImVec2(0,0), ImGuiInputTextFlags_EnterReturnsTrue);
    EndInput();
    return hasChanged;
}           
