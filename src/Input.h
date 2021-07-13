#ifndef _INPUT

#include <stdint.h>

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

private:
	struct ActionStrct {
		int key;
		Modifier modifiers;
	};

	static ActionStrct actionsArr[Action_COUNT];
	static ActionStrct actionsArrDefault[Action_COUNT];

	static void initActionsArrToDefault();
	static void initDefaultActionsArr();

	static Modifier getModifiers();

	static int toLocalKey(int key);
public:
	static void init();

	static int mainMouseB();
	static int secMouseB();
	static int midMouseB();

	static bool modShift();
	static bool modCtrl();
	static bool modAlt();

	static bool isActionActive(Action action, ActionState actionState = ActionState_Pressed);

	static void changeActionBinding(Action action, int key, Modifier mods);
	static void setActionBindingToDefault(Action action);
};

#endif // !_INPUT
