#ifndef _IMGUI_EXT
#define _IMGUI_EXT

#include <stdint.h>
#include "imgui.h"
#include "raylib.h"

ImVec2 operator+(const ImVec2& a, const ImVec2& b);

namespace ImGuiExt {
	Color imColortoRaylib(const ImVec4& c);
	Color rlVec4toColor(const Vector4& c);

	bool imguiColorPickerButton(const char* name, ImVec4& color, ImVec2 size = { 50,0 });
	void TextUnformattedCentered(const char* text);
	void TextColoredCentered(const ImVec4& color, const char* text);
	uint32_t SelectSwitch(const char*const* labels, size_t num, uint32_t selected, int id, const ImVec2& size);
}

#endif