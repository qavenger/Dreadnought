#pragma once
#define MAX_KEY_NUM 256

DEFINE_DELEGATE_TWO_PARAM(KeyCharEvent, TCHAR, bool);
DEFINE_DELEGATE_ONE_PARAM(MouseEvent, int2);
DEFINE_DELEGATE_ONE_PARAM(ProcessBufferEvent, std::wstring);
DEFINE_DELEGATE_NO_PARAM(KeyInputEvent);

class Input
{
public:
	enum class EMouseButton
	{
		Left = 0,
		Middle,
		Right,

		Invalid
	};
	enum class EKeyCode
	{
		LMouse = 0x01, RMouse, Cancel, MMouse, XButton1, XButton2,
		Back = 0x08, Tab,
		Clear = 0x0C, Return,
		Shift = 0x10, Control, Menu, Pause, Caps_Lock,
		Escape = 0x1B,
		Space = 0x20, Page_Up, Page_Down, End, Home, Left, Up, Right, Down, Select,
		Print, Execute, Print_Screen, Insert, Delete, Help,
		Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,
		A = 0x41,
		B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		LWin, RWin, App,
		Sleep = 0x5F,
		NumPad0, NumPad1, NumPad2, NumPad3, NumPad4,
		NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
		NumPadMultiply, NumPadAdd, NumPadSeparator, NumPadSubtract, NumPadDecimal, NumPadDivide,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20,
		F21, F22, F23, F24,
		Num_Lock = 0x90, Scroll,
		LShift = 0xA0, RShift,
		LControl, RControl, LMenu, RMenu, Browser_Back,
		Browser_Forward, Browser_Refresh, Browser_Stop,
		Browser_Search, Browser_Favorites, Browser_Home,
		Volumn_Mute, Volumn_Down, Volumn_Up,
		Media_Next_Track, Media_Previous_Track, Media_Stop, Media_Play_Pause,
		Semicolon = 0xBA, Add , Comma, Subtract, Period, Divide,
		Tilde = 0xC0,
		NUM_KEYCODE
	};
	static void BindKeyCharEvent(void (*p)(TCHAR, bool));
	static void UnBindKeyCharEvent(void (*p)(TCHAR, bool));
	static bool IsBoundKeyCharEvent(void (*p)(TCHAR, bool));
	static bool OnKeyChar(const TCHAR character, bool bIsRepeat);
	static void BindKeyInput(EKeyCode keyCode, EKeyInputState keyState, void (*p)());
	static void UnBindKeyInput(EKeyCode keyCode, EKeyInputState keyState, void (*p)());
	static bool OnKeyInput(EKeyCode keyCode, EKeyInputState keyState);
	static void BindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void (*p)(int2));
	static void UnBindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void (*p)(int2));
	static bool OnMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, int x, int y);
	static void BindMouseMoveEvent(void (*p)(int2));
	static void UnBindMouseMoveEvent(void (*p)(int2));
	static bool OnMouseMove(int x, int y);

	static bool IsKeyDown(EKeyCode keyCode);
	static bool IsKeyUp(EKeyCode keyCode);
	static bool IsMouseButtonDown(EMouseButton button) { return MouseInputState[(int)button] != EMouseInputState::UP; }
	static bool IsMouseButtonUp(EMouseButton button) { return MouseInputState[(int)button] == EMouseInputState::UP; }

	static int2 GetMousePosition();
	static int2 GetScreenMousePosition();
	static int2 GetMouseRelativePosition();
	static void Start(HWND hWnd);
	static void UpdateMouse(EMouseButton keyCode, EMouseInputState mouseState, int x, int y);

private:
	static KeyCharEvent OnKeyCharEvent;
	static KeyInputEvent OnKeyInputEvent[(uint8)EKeyInputState::NUM][MAX_KEY_NUM];
	static MouseEvent OnMouseButtonEvent[(int)EMouseInputState::NUM][(int)EMouseButton::Invalid];
	static MouseEvent OnMouseMoveEvent;
	static EMouseInputState MouseInputState[(int)EMouseButton::Invalid];
	static POINT m_prevMousePos;
	static POINT m_mousePos;
	static HWND m_hWnd;
};

