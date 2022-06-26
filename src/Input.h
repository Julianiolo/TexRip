#ifndef _INPUT

#include <stdint.h>
#include <string>
#include <vector>
#include "imgui.h"

class Input {
public:
	typedef int Action;

	enum ACTIONS {
		Action_move = 0,
		Action_rotate,
		Action_scale,
		Action_selectAll,
		Action_selectLinked,
		Action_add,
		Action_remove,
		Action_undo,
		Action_save,
		Action_open,
		Action_COUNT
	};

	typedef uint8_t ActionState;

	enum ACTION_STATES {
		ActionState_Pressed = 0,
		ActionState_Released,
		ActionState_Down,
		ActionState_Up
	};

	typedef uint8_t Modifier;
	enum MODIFIERS {
		Modifier_None  =      0,
		Modifier_Shift = (1<<0),
		Modifier_Ctrl  = (1<<1),
		Modifier_Alt   = (1<<2),
	};

	enum MOUSE_ACTIONS{
		MouseAction_Select = 0,
		MouseAction_Cancel,
		MouseAction_MoveCam,
		MouseAction_COUNT
	};

private:
	static constexpr const char* mouseButtonLabels[] = {"Left","Middle","Right"};

	struct ActionStrctKeys {
		int key;
		int localKey;
		Modifier modifiers;
		std::string keyName;
	};

	struct ActionStrct {
		ActionStrctKeys def;
		ActionStrctKeys changed;
		std::string label;
	};

	class ActionKeyCapture {
	private:
		static bool active;
		static ActionStrctKeys captured;
		static Action target;
		static std::string str;
		static bool finished;

		static void stopCapture(bool cancel);
	public:
		static void startCapture(Action targ);

		static void captureKeys();
		static bool isCapturing();
	};

	typedef uint32_t MouseAction;
	struct MouseActionStrct{
		int button;
		int defButton;
	};
	

	static ActionStrct actionsArr[Action_COUNT];
	static bool inputEnabled;

	static MouseActionStrct mouseActionsArr[MouseAction_COUNT];

	static void initActionsArrToDefault();
	static void initDefaultActionsArr();

	static Modifier getModifiers();

	static int toLocalKey(int key);
	static std::string generateActionKeyName(const ActionStrctKeys& a);

	static void drawMouseSelectSwitchPro(MouseAction action);
public:
	static void init();

	static bool modShift();
	static bool modCtrl();
	static bool modAlt();

	static bool isActionActive(Action action, ActionState actionState = ActionState_Pressed);
	static bool isMouseActionActive(MouseAction action, ActionState actionState = ActionState_Pressed);

	static const std::string& getActionKeyName(Action action);

	static void changeActionBinding(Action action, int key, Modifier mods);
	static void setActionBindingToDefault(Action action);

	static void enableInput(bool enable);

	static void drawSettingsTable();
};

#endif // !_INPUT
