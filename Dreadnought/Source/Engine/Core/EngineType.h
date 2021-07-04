#pragma once
enum class EKeyInputState : uint8 { PRESSED, RELEASED, NUM };

enum class EMouseInputState : uint8 { DOWN, UP, DBLCLK, NUM, MOVE };

enum class EMouseButton : int8 { Left = 1, Middle = 4, Right = 2, Invalid = 3};

enum class EKeyCode : uint8
{
	LMouse = 0x01, RMouse, Cancel, MMouse, XButton1, XButton2,
	Back = 0x08, Tab,
	Clear = 0x0C, Return,
	Shift = 0x10, Control, Alt, Pause, Caps_Lock,
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
	LControl, RControl, LAlt, RAlt, Browser_Back,
	Browser_Forward, Browser_Refresh, Browser_Stop,
	Browser_Search, Browser_Favorites, Browser_Home,
	Volumn_Mute, Volumn_Down, Volumn_Up,
	Media_Next_Track, Media_Previous_Track, Media_Stop, Media_Play_Pause,
	Semicolon = 0xBA, Add, Comma, Subtract, Period, Divide,
	Tilde = 0xC0,
	NUM_KEYCODE
};
enum class ESystemKeyCode
{
	None = 0x00,
	Shift = 0x10, Control, Alt,
	LShift = 0xA0, RShift,
	LControl, RControl, LAlt, RAlt,
};

struct BGRAColor
{
	uint8 A;
	uint8 R;
	uint8 G;
	uint8 B;
};