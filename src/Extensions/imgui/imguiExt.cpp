#include "imguiExts.h"
#include <stdint.h>
#include <string>

ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
    return ImVec2{ a.x + b.x, a.y + b.y };
}

Color ImGuiExt::imColortoRaylib(const ImVec4& c) {
    return { (uint8_t)(c.x*255) , (uint8_t)(c.y*255), (uint8_t)(c.z*255), (uint8_t)(c.w*255) };
}

Color ImGuiExt::rlVec4toColor(const Vector4& c) {
    return { (uint8_t)(c.x*255) , (uint8_t)(c.y*255), (uint8_t)(c.z*255), (uint8_t)(c.w*255) };
}

bool ImGuiExt::imguiColorPickerButton(const char* name, ImVec4& color, ImVec2 size) {
    bool changed = false;
    std::string nameStr = name;

    ImGui::AlignTextToFramePadding();
    if (ImGui::ColorButton(name, color, ImGuiColorEditFlags_AlphaPreview, size)) {
        ImGui::OpenPopup((nameStr + "_edit_popup").c_str());
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(name);

    if (ImGui::BeginPopup((nameStr + "_edit_popup").c_str())) {
        changed = ImGui::ColorPicker4(name, (float*)&color, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
        ImGui::EndPopup();
    }
    return changed;
}

void ImGuiExt::TextUnformattedCentered(const char* text) {
    float contentWidth = ImGui::GetContentRegionAvail().x;
    auto textWidth   = ImGui::CalcTextSize(text).x;

    ImGui::SetCursorPosX((contentWidth - textWidth) * 0.5f);
    ImGui::TextUnformatted(text);
}
void ImGuiExt::TextColoredCentered(const ImVec4& color, const char* text) {
    float contentWidth = ImGui::GetContentRegionAvail().x;
    auto textWidth     = ImGui::CalcTextSize(text).x;

    ImGui::SetCursorPosX(contentWidth/2 - textWidth/2);
    ImGui::TextColored(color,text);
}

uint32_t ImGuiExt::SelectSwitch(const char*const* labels, size_t num, uint32_t selected, int id, const ImVec2& size) {
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.5,0.5 });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0,0});
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0,0});
    ImGui::BeginChildFrame(ImGui::GetID(&id+id),size);
    int newSelection = -1;
    for(size_t i = 0; i < num; i++){
        if (i) {
            ImGui::SameLine();
        }
       
        if (ImGui::Selectable(labels[i], selected == i, ImGuiSelectableFlags_DontClosePopups, {size.x / num, size.y}))
            newSelection = i;
    }
    ImGui::EndChildFrame();
    ImGui::PopStyleVar(3);
    return newSelection != -1 ? newSelection : selected;
}