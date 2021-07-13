#include "Input.h"
#include "raylib.h"
#include "external/glfw/include/GLFW/glfw3.h"

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
	char k = glfwGetKeyName(key, 0)[0];
	if (k >= 'a' && k <= 'z') {
		k -= 'a' - 'A';
	}
	return k;
}

Input::ActionStrct Input::actionsArr[Input::Action_COUNT];
Input::ActionStrct Input::actionsArrDefault[Input::Action_COUNT];

void Input::init() {
	initDefaultActionsArr();
	initActionsArrToDefault();
}

void Input::initActionsArrToDefault() {
	for (int i = 0; i < Action_COUNT; i++) {
		actionsArr[i] = actionsArrDefault[i];
	}
}

void Input::initDefaultActionsArr() {
	actionsArrDefault[Action_move] = { KEY_G, Modifier_None};
	actionsArrDefault[Action_rotate] = { KEY_R, Modifier_None};
	actionsArrDefault[Action_scale] = { KEY_S, Modifier_None};

	actionsArrDefault[Action_selectAll] = { KEY_A, Modifier_None};
	actionsArrDefault[Action_selectLinked] = { KEY_L, Modifier_None};

	actionsArrDefault[Action_add] = { KEY_A, Modifier_Shift};

	actionsArrDefault[Action_undo] = { KEY_Z, Modifier_Ctrl};
	actionsArrDefault[Action_save] = { KEY_S, Modifier_Ctrl};
	actionsArrDefault[Action_open] = { KEY_O, Modifier_Ctrl};

	for (size_t i = 0; i < Action_COUNT; i++) {
		actionsArrDefault[i].key = toLocalKey(actionsArrDefault[i].key);
	}
}

bool Input::isActionActive(Action action, ActionState actionState) {
	ActionStrct& acStrct = actionsArr[action];
	Modifier currentMods = getModifiers();
	if (!((currentMods & acStrct.modifiers) == acStrct.modifiers)) { // modifiers are not satisfied
		return false;
	}

	switch (actionState) {
		case ActionState_Pressed:
			return IsKeyPressed(acStrct.key);
		case ActionState_Released:
			return IsKeyReleased(acStrct.key);
		case ActionState_Down:
			return IsKeyDown(acStrct.key);
		case ActionState_Up:
			return IsKeyPressed(acStrct.key);
	}
	return false;
}

void Input::changeActionBinding(Action action, int key, Modifier mods) {
	actionsArr[action] = ActionStrct{toLocalKey(key), mods};
}
void Input::setActionBindingToDefault(Action action) {
	actionsArr[action] = actionsArrDefault[action];
}

/*
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