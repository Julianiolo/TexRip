#include "utils.h"
#include <stdint.h>
#include "../oneHeaderLibs/VectorOperators.h"
#include <cctype>                                   // for isprint


Vector2 utils::rotatePoint(Vector2 pnt, const Vector2& center, float s, float c) {
    pnt -= center;
    Vector2 newPnt = {pnt.x*c - pnt.y*s, pnt.x*s + pnt.y*c};
    newPnt += center;
    return newPnt;
}

std::string utils::getKeyStr(int key) {
	if (key == KEY_NULL) {
		return "";
	}

	if (isprint(key) && key != KEY_SPACE && key != KEY_TAB) {
		if (key >= 'a' && key <= 'z') {
			return std::string(1,(char)(key- ('a' - 'A')));
		}
		else {
			return std::string(1,(char)key);
		}
	}
	else {
		switch (key) {
			case KEY_BACK:
				return "BACKSPACE";
			case KEY_BACKSPACE:
				return "BACKSPACE";
			case KEY_CAPS_LOCK:
				return "CAPS_LOCK";
			case KEY_DELETE:
				return "DELETE";
			case KEY_DOWN:
				return "DOWN";
			case KEY_END:
				return "END";
			case KEY_ENTER:
				return "ENTER";
			case KEY_ESCAPE:
				return "ESCAPE";
			case KEY_F1:
				return "F1";
			case KEY_F2:
				return "F2";
			case KEY_F3:
				return "F3";
			case KEY_F4:
				return "F4";
			case KEY_F5:
				return "F5";
			case KEY_F6:
				return "F6";
			case KEY_F7:
				return "F7";
			case KEY_F8:
				return "F8";
			case KEY_F9:
				return "F9";
			case KEY_F10:
				return "F10";
			case KEY_F11:
				return "F11";
			case KEY_F12:
				return "F12";
			case KEY_HOME:
				return "HOME";
			case KEY_INSERT:
				return "INSERT";
			case KEY_KB_MENU:
				return "KB_MENU";
			case KEY_KP_0:
				return "KP_0";
			case KEY_KP_1:
				return "KP_1";
			case KEY_KP_2:
				return "KP_2";
			case KEY_KP_3:
				return "KP_3";
			case KEY_KP_4:
				return "KP_4";
			case KEY_KP_5:
				return "KP_5";
			case KEY_KP_6:
				return "KP_6";
			case KEY_KP_7:
				return "KP_7";
			case KEY_KP_8:
				return "KP_8";
			case KEY_KP_9:
				return "KP_9";
			case KEY_KP_ADD:
				return "KP_ADD";
			case KEY_KP_DECIMAL:
				return "KP_DECIMAL";
			case KEY_KP_DIVIDE:
				return "KP_DIVIDE";
			case KEY_KP_ENTER:
				return "KP_ENTER";
			case KEY_KP_EQUAL:
				return "KP_EQUAL";
			case KEY_KP_MULTIPLY:
				return "KP_MULTIPLY";
			case KEY_KP_SUBTRACT:
				return "KP_SUBTRACT";
			case KEY_LEFT:
				return "LEFT";
			case KEY_LEFT_ALT:
				return "LEFT_ALT";
			case KEY_LEFT_CONTROL:
				return "LEFT_CONTROL";
			case KEY_LEFT_SHIFT:
				return "LEFT_SHIFT";
			case KEY_LEFT_SUPER:
				return "LEFT_SUPER";
			case KEY_MENU:
				return "MENU";
			case KEY_NUM_LOCK:
				return "NUM_LOCK";
			case KEY_PAGE_DOWN:
				return "PAGE_DOWN";
			case KEY_PAGE_UP:
				return "PAGE_UP";
			case KEY_PAUSE:
				return "PAUSE";
			case KEY_PRINT_SCREEN:
				return "PRINT_SCREEN";
			case KEY_RIGHT:
				return "RIGHT";
			case KEY_RIGHT_ALT:
				return "RIGHT_ALT";
			case KEY_RIGHT_CONTROL:
				return "RIGHT_CONTROL";
			case KEY_RIGHT_SHIFT:
				return "RIGHT_SHIFT";
			case KEY_RIGHT_SUPER:
				return "RIGHT_SUPER";
			case KEY_SCROLL_LOCK:
				return "SCROLL_LOCK";
			case KEY_SPACE:
				return "SPACE";
			case KEY_TAB:
				return "TAB";
			case KEY_UP:
				return "UP";
			case KEY_VOLUME_DOWN:
				return "VOLUME_DOWN";
			case KEY_VOLUME_UP:
				return "VOLUME_UP";
			default:
				return "UNKNOWN";
		}
	}
}