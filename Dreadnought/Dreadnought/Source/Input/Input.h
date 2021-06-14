#pragma once
#define MAX_KEY_NUM 256

DEFINE_DELEGATE_ONE_PARAM(KeyCharEvent, TCHAR);
DEFINE_DELEGATE_ONE_PARAM(MouseInputEvent, int2);
DEFINE_DELEGATE_ONE_PARAM(ProcessBufferEvent, std::wstring);
DEFINE_DELEGATE_NO_PARAM(KeyInputEvent);

class Input
{
	friend class Engine;
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
		Semicolon = 0xBA, Add , Comma, Subtract, Period, Divide,
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


	class KeyEvent
	{
	public:
		KeyEvent() :
			State(EKeyInputState::NUM),
			KeyCode(0)
		{}
		KeyEvent(EKeyInputState state, EKeyCode keyCode);
		
		EKeyInputState GetState()const { return State; }
		unsigned char GetKeyCode()const { return KeyCode; }
	private:
		EKeyInputState State;
		unsigned char KeyCode;
	};

	class MouseEvent
	{
	public:
		MouseEvent() :
			State(EMouseInputState::NUM),
			Button(EMouseButton::Invalid),
			Loc({0,0})
		{
		}
		MouseEvent(EMouseInputState state, EMouseButton button, int2 loc);
		MouseEvent(int2 loc);
		EMouseInputState GetState()const { return State; }
		EMouseButton GetButton()const { return Button; }
		int2 GetPosition()const { return Loc; }
	private:
		EMouseInputState State;
		EMouseButton Button;
		int2 Loc;
	};

	class MouseRawEvent
	{
	public:
		MouseRawEvent() : Vel({0,0}){}
		MouseRawEvent(int2 vel) : Vel(vel) {}
		int2 GetVelocity()const { return Vel; }
	private:
		int2 Vel;
	};

	static void BindKeyCharEvent(void (*p)(TCHAR));
	static void UnBindKeyCharEvent(void (*p)(TCHAR));
	static bool IsBoundKeyCharEvent(void (*p)(TCHAR));
	static void BindKeyInput(EKeyCode keyCode, EKeyInputState keyState, void (*p)());
	static void UnBindKeyInput(EKeyCode keyCode, EKeyInputState keyState, void (*p)());
	static void BindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void (*p)(int2));
	static void UnBindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void (*p)(int2));
	static void BindMouseMoveEvent(void (*p)(int2));
	static void UnBindMouseMoveEvent(void (*p)(int2));
	static void BindMouseRawMoveEvent(void (*p)(int2));
	static void UnBindMouseRawMoveEvent(void (*p)(int2));

	static bool IsKeyDown(EKeyCode keyCode);
	static bool IsKeyUp(EKeyCode keyCode);
	static bool IsMouseButtonDown(EMouseButton button) { return MouseInputState[(int)button] != EMouseInputState::UP; }
	static bool IsMouseButtonUp(EMouseButton button) { return MouseInputState[(int)button] == EMouseInputState::UP; }

	static int2 GetMousePosition();
	static int2 GetScreenMousePosition();
	static int2 GetMouseRelativePosition();
	static void Start(HWND hWnd);
	static void ClearKeyEvent();
	static void ClearCharEvent();
	static void ClearKeyboardEvents();
	static void EnableAutoRepeat();
	static void DisableAutoRepeat();
	static bool IsAutoRepeatEnabled();
private:
	static void OnKeyInput(EKeyCode keyCode, EKeyInputState keyState);
	static void OnKeyChar(const TCHAR character, bool bIsRepeat); 
	static void OnMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, int x, int y);
	static void OnMouseMove(int x, int y);
	static void OnMouseRawMove(int x, int y);
	template<typename T>
	void TrimBuffer(std::queue<T>& buffer)
	{
		while (buffer.size() > BufferSize)
			buffer.pop();
	}
private:
	static void ProcessKeyborad();
	static void ProcessMouse();
private:
	static void Tick();
private:
	static KeyCharEvent OnKeyCharEvent;
	static KeyInputEvent OnKeyInputEvent[(uint8)EKeyInputState::NUM][(unsigned long long)Input::EKeyCode::NUM_KEYCODE];
	static MouseInputEvent OnMouseButtonEvent[(int)EMouseInputState::NUM][(int)EMouseButton::Invalid];
	static MouseInputEvent OnMouseMoveEvent;
	static MouseInputEvent OnMouseRawMoveEvent;
	static EMouseInputState MouseInputState[(int)EMouseButton::Invalid];
	static POINT m_prevMousePos;
	static POINT m_mousePos;
	static HWND m_hWnd;
	static std::vector<char> RawInputBuffer;
	static constexpr unsigned int BufferSize = 16;
	static std::queue<KeyEvent> KeyBuffer;
	static std::queue<MouseEvent> MouseBuffer;
	static std::queue<TCHAR> CharBuffer;
	static std::queue<MouseRawEvent> MouseRawBuffer;
	static bool bAutoRepeatEnable;
	static bool bEnableRawInput;
};

