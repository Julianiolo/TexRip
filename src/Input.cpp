#include "Input.h"

#include "raylib.h"
#include "utils/utils.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "Extensions/imgui/imguiExts.h"

#if defined(PLATFORM_DESKTOP)
	#include "external/glfw/include/GLFW/glfw3.h"
#endif



bool Input::modShift() {
	return inputEnabled && IsKeyDown(KEY_LEFT_SHIFT);
}
bool Input::modCtrl() {
	return inputEnabled && IsKeyDown(KEY_LEFT_CONTROL);
}
bool Input::modAlt() {
	return inputEnabled && IsKeyDown(KEY_LEFT_ALT);
}

Input::Modifier Input::getModifiers() {
	Modifier mods = Modifier_None;
	if (modShift())
		mods |= Modifier_Shift;
	if (modCtrl())
		mods |= Modifier_Ctrl;
	if (modAlt())
		mods |= Modifier_Alt;
	return mods;
}

int Input::toLocalKey(int key) {
#if defined(PLATFORM_DESKTOP)
	if (isprint(key)) {
		char k = glfwGetKeyName(key, 0)[0];
		if (k >= 'a' && k <= 'z') {
			k -= 'a' - 'A';
		}
		return k;
	}
	else {
		return key;
	}
#else
	return key;
#endif
}

Input::ActionStrct Input::actionsArr[Input::Action_COUNT];
bool Input::inputEnabled = true;

Input::MouseActionStrct Input::mouseActionsArr[Input::MouseAction_COUNT];

void Input::init() {
	initDefaultActionsArr();
	initActionsArrToDefault();
}

inline void Input::initActionsArrToDefault() {
	for (size_t i = 0; i < MouseAction_COUNT; i++) {
		mouseActionsArr[i].button = mouseActionsArr[i].defButton;
	}
	
	for (size_t i = 0; i < Action_COUNT; i++) {
		actionsArr[i].changed = actionsArr[i].def;
	}
}

#define setDefaultAction(action, key, mods) {\
	actionsArr[action].def = {key, toLocalKey(key), mods}; \
	actionsArr[action].def.keyName = generateActionKeyName(actionsArr[action].def); \
	actionsArr[action].label = std::string(#action).replace(0,7,"");\
}
inline void Input::initDefaultActionsArr() {
	mouseActionsArr[MouseAction_Select].defButton = MOUSE_BUTTON_LEFT;
	mouseActionsArr[MouseAction_Cancel].defButton = MOUSE_BUTTON_RIGHT;
	mouseActionsArr[MouseAction_MoveCam].defButton = MOUSE_BUTTON_MIDDLE;

	setDefaultAction(Action_move,   KEY_G, Modifier_None);
	setDefaultAction(Action_rotate, KEY_R, Modifier_None);
	setDefaultAction(Action_scale,  KEY_S, Modifier_None);

	setDefaultAction(Action_selectAll, KEY_A, Modifier_None);
	setDefaultAction(Action_selectLinked, KEY_L, Modifier_None);

	setDefaultAction(Action_add, KEY_A, Modifier_Shift);

	setDefaultAction(Action_undo, KEY_Z, Modifier_Ctrl);
	setDefaultAction(Action_save, KEY_S, Modifier_Ctrl);
	setDefaultAction(Action_open, KEY_O, Modifier_Ctrl);
}

std::string Input::generateActionKeyName(const ActionStrctKeys& a) {
	std::string out = "";
	if (a.modifiers & Modifier_Ctrl)
		out += "CTRL+";
	if (a.modifiers & Modifier_Alt)
		out += "ALT+";
	if (a.modifiers & Modifier_Shift)
		out += "SHIFT+";

	return out + utils::getKeyStr(a.localKey);
}

bool Input::isActionActive(Action action, ActionState actionState) {
	if (!inputEnabled)
		return false;

	const ActionStrct& acStrct = actionsArr[action];
	const Modifier currentMods = getModifiers();
	if (!((currentMods & acStrct.changed.modifiers) == acStrct.changed.modifiers)) { // modifiers are not satisfied
		return false;
	}

	int key = acStrct.changed.localKey;

	switch (actionState) {
		case ActionState_Pressed:
			return IsKeyPressed(key);
		case ActionState_Released:
			return IsKeyReleased(key);
		case ActionState_Down:
			return IsKeyDown(key);
		case ActionState_Up:
			return IsKeyPressed(key);
	}
	return false;
}

bool Input::isMouseActionActive(MouseAction action, ActionState actionState){
	const MouseActionStrct& strct = mouseActionsArr[action];
	int button = strct.button;
	
	switch (actionState) {
		case ActionState_Pressed:
			return IsMouseButtonPressed(button);
		case ActionState_Released:
			return IsMouseButtonReleased(button);
		case ActionState_Down:
			return IsMouseButtonDown(button);
		case ActionState_Up:
			return IsMouseButtonUp(button);
	}
	return false;
}

const std::string& Input::getActionKeyName(Action action) {
	return actionsArr[action].changed.keyName;
}

void Input::changeActionBinding(Action action, int key, Modifier mods) {
	actionsArr[action].changed = ActionStrctKeys{key,toLocalKey(key), mods};
	actionsArr[action].changed.keyName = generateActionKeyName(actionsArr[action].changed);
}
void Input::setActionBindingToDefault(Action action) {
	actionsArr[action].changed = actionsArr[action].def;
}

void Input::enableInput(bool enable) {
	inputEnabled = enable;
}

void Input::drawMouseSelectSwitchPro(MouseAction action){
	constexpr int buttonMap[] =    {MOUSE_BUTTON_LEFT, MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_RIGHT};
	constexpr int buttonMapRev[] = {                0,                   2,                  1};
	int button = buttonMapRev[mouseActionsArr[action].button];
	int newButton = (int)ImGuiExt::SelectSwitch(mouseButtonLabels,3,button,action,{200,20});
	mouseActionsArr[action].button = buttonMap[newButton];
}

void Input::drawSettingsTable() {
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
	if(ImGui::TreeNode("Mouse")){
		
		ImGui::TextUnformatted("Select:");
		drawMouseSelectSwitchPro(MouseAction_Select);
		
		ImGui::TextUnformatted("Cancel:");
		drawMouseSelectSwitchPro(MouseAction_Cancel);

		ImGui::TextUnformatted("Move:");
		drawMouseSelectSwitchPro(MouseAction_MoveCam);
		
		ImGui::TreePop();
	}

	if(ActionKeyCapture::isCapturing())
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Always);
	else
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Appearing);
	if(ImGui::TreeNode("Keyboard")){
		ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_PadOuterX | ImGuiTableFlags_BordersV;

		Action act = -1;
		if (ImGui::BeginTable("Key Actions Table", 3, flags)) {
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Binding", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for (size_t i = 0; i < Action_COUNT; i++) { //ImGui::AlignTextToFramePadding();
				ActionStrct& a = actionsArr[i];

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(a.label.c_str());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(a.changed.keyName.c_str());
				
				ImGui::TableNextColumn();
				if (ImGui::Button("Change")) {
					act = i;
				}
				ImGui::SameLine();
				bool isAlreadyDefault = (a.changed.key == a.def.key) && (a.changed.modifiers == a.def.modifiers);
				if (isAlreadyDefault)
					ImGui::PushDisabled();
				if (ImGui::Button("Default")) {
					setActionBindingToDefault(i);
				}
				if (isAlreadyDefault)
					ImGui::PopDisabled();
			}
			ImGui::EndTable();
		}
		if (act != -1) {
			ActionKeyCapture::startCapture(act);
		}

		ActionKeyCapture::captureKeys();
		ImGui::TreePop();
	}
}

bool Input::ActionKeyCapture::active = false;
Input::ActionStrctKeys Input::ActionKeyCapture::captured;
Input::Action Input::ActionKeyCapture::target = -1;
bool Input::ActionKeyCapture::finished = false;

void Input::ActionKeyCapture::captureKeys() {
	if (active) {
		if (!finished) {
			int key;
			bool changed = false;
			while ( (key = GetKeyPressed()) ) {
				changed = true;
				if (key < 255 && isprint(key)) {
					finished = true;
					captured.key = key;
					captured.localKey = toLocalKey(key);
					break;
				}
				else {
					if (key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT)
						captured.modifiers |= Modifier_Shift;
					if (key == KEY_LEFT_CONTROL || key == KEY_RIGHT_CONTROL)
						captured.modifiers |= Modifier_Ctrl;
					if (key == KEY_LEFT_ALT || key == KEY_RIGHT_ALT)
						captured.modifiers |= Modifier_Alt;
				}
			}
			if (changed) {
				captured.keyName = generateActionKeyName(captured);
			}
		}
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Enter Keys", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings)) {
		ImGuiExt::TextColoredCentered(ImVec4(0.1f, 0.9f, 0.2f, 1.0f), captured.keyName.c_str());

		if (!finished)
			ImGui::PushDisabled();
		if (ImGui::Button("Ok", ImVec2(120, 0))) {
			stopCapture(false);
			ImGui::CloseCurrentPopup();
		}
		if (!finished)
			ImGui::PopDisabled();

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			stopCapture(true);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void Input::ActionKeyCapture::startCapture(Action targ) {
	enableInput(false);
	active = true;
	finished = false;
	target = targ;
	captured = { 0,0,0,"" };
	ImGui::OpenPopup("Enter Keys");
}
void Input::ActionKeyCapture::stopCapture(bool cancel) {
	enableInput(true);
	active = false;
	if(!cancel)
		changeActionBinding(target, captured.key, captured.modifiers);
}

bool Input::ActionKeyCapture::isCapturing(){
	return active;
}

/*

void Input::setDefaultAction(Action a, int key, Modifier mods) {
actionsArrDefault[a] = {key, toLocalKey(key), mods};
generateActionKeyName(&actionsArrDefault[a]);
}

actionsArrDefault[Action_move] = { KEY_G, Modifier_None};
actionsArrDefault[Action_rotate] = { KEY_R, Modifier_None};
actionsArrDefault[Action_scale] = { KEY_S, Modifier_None};

actionsArrDefault[Action_selectAll] = { KEY_A, Modifier_None};
actionsArrDefault[Action_selectLinked] = { KEY_L, Modifier_None};

actionsArrDefault[Action_add] = { KEY_A, Modifier_Shift};

actionsArrDefault[Action_undo] = { KEY_Z, Modifier_Ctrl};
actionsArrDefault[Action_save] = { KEY_S, Modifier_Ctrl};
actionsArrDefault[Action_open] = { KEY_O, Modifier_Ctrl};

bool Input::selAll() {
	return IsKeyPressed(KEY_A);
}
bool Input::selLinked() {
	return IsKeyPressed(KEY_L);
}

bool Input::move() {
	return IsKeyPressed(KEY_G);
}
bool Input::rotate() {
	return IsKeyPressed(KEY_R);
}
bool Input::scale() {
	return IsKeyPressed(KEY_S);
}

bool Input::undo() {
	return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Y);
}

bool Input::save() {
	return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S);
}
bool Input::open() {
	return IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O);
}
*/