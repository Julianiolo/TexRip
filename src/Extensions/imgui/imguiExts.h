#ifndef _IMGUI_EXT
#define _IMGUI_EXT

#include "imgui.h"
#include "raylib.h"

ImVec2 operator+(const ImVec2& a, const ImVec2& b);

namespace imguiExt {
	Color imColortoRaylib(const ImVec4& c);
	Color rlVec4toColor(const Vector4& c);

	bool imguiColorPickerButton(const char* name, ImVec4& color, ImVec2 size = { 50,0 });
}

#endif