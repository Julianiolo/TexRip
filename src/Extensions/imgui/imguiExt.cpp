#include "imguiExts.h"
#include <stdint.h>
#include <string>

ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
    return ImVec2{ a.x + b.x, a.y + b.y };
}

Color imguiExt::imColortoRaylib(const ImVec4& c) {
    return { (uint8_t)(c.x*255) , (uint8_t)(c.y*255), (uint8_t)(c.z*255), (uint8_t)(c.w*255) };
}

Color imguiExt::rlVec4toColor(const Vector4& c) {
    return { (uint8_t)(c.x*255) , (uint8_t)(c.y*255), (uint8_t)(c.z*255), (uint8_t)(c.w*255) };
}

bool imguiExt::imguiColorPickerButton(const char* name, ImVec4& color, ImVec2 size) {
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