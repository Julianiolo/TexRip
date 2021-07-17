#include "Input.h"
#include "raylib.h"
#include "utils/utils.h"

#if defined(PLATFORM_DESKTOP)
	#include "external/glfw/include/GLFW/glfw3.h"
#endif

int Input::mainMouseB() {
	return MOUSE_BUTTON_LEFT;
}
int Input::secMouseB() {
	return MOUSE_BUTTON_RIGHT;
}
int Input::midMouseB() {
	return MOUSE_BUTTON_MIDDLE;
}

bool Input::modShift() {
	return IsKeyDown(KEY_LEFT_SHIFT);
}
bool Input::modCtrl() {
	return IsKeyDown(KEY_LEFT_CONTROL);
}
bool Input::modAlt() {
	return IsKeyDown(KEY_LEFT_ALT);
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
Input::ActionStrct Input::actionsArrDefault[Input::Action_COUNT];

void Input::init() {
	initDefaultActionsArr();
	initActionsArrToDefault();
}

inline void Input::initActionsArrToDefault() {
	for (int i = 0; i < Action_COUNT; i++) {
		actionsArr[i] = actionsArrDefault[i];
	}
}
inline void Input::initDefaultActionsArr() {
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
void Input::setDefaultAction(Action a, int key, Modifier mods) {
	actionsArrDefault[a] = {key, toLocalKey(key), mods};
	generateActionKeyName(&actionsArrDefault[a]);
}

void Input::generateActionKeyName(ActionStrct* a) {
	std::string out = "";
	if (a->modifiers & Modifier_Ctrl)
		out += "CTRL+";
	if (a->modifiers & Modifier_Alt)
		out += "ALT+";
	if (a->modifiers & Modifier_Shift)
		out += "SHIFT+";

	a->keyName = out + utils::getKeyStr(a->localKey);
}

bool Input::isActionActive(Action action, ActionState actionState) {
	ActionStrct& acStrct = actionsArr[action];
	Modifier currentMods = getModifiers();
	if (!((currentMods & acStrct.modifiers) == acStrct.modifiers)) { // modifiers are not satisfied
		return false;
	}

	int key = acStrct.localKey;

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

const std::string& Input::getActionKeyName(Action action) {
	return actionsArr[action].keyName;
}

void Input::changeActionBinding(Action action, int key, Modifier mods) {
	actionsArr[action] = ActionStrct{key,toLocalKey(key), mods};
	generateActionKeyName(&actionsArr[action]);
}
void Input::setActionBindingToDefault(Action action) {
	actionsArr[action] = actionsArrDefault[action];
}

/*

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