#pragma once
class Input : public ISubsystem
{
	friend class Engine;
	DEFINE_DELEGATE_ONE_PARAM(KeyCharEventDelegate, tchar);
	DEFINE_DELEGATE_ONE_PARAM(MouseWheelEventDelegate, float);
	DEFINE_DELEGATE_ONE_PARAM(MouseInputEventDelegate, int2);
	DEFINE_DELEGATE_NO_PARAM(KeyInputEventDelegate);
	class KeyboardEvent
	{
	public:
		KeyboardEvent() : State(EKeyInputState::NUM), KeyCode(EKeyCode::NUM_KEYCODE) {}
		KeyboardEvent(EKeyInputState state, EKeyCode keyCode) : State(state), KeyCode(keyCode) {}

		EKeyInputState GetState()const { return State; }
		EKeyCode GetKeyCode()const { return KeyCode; }
	private:
		EKeyInputState State;
		EKeyCode KeyCode;
	};

	class MouseEvent
	{
	public:
		MouseEvent() : State(EMouseInputState::NUM), Button(-1), Loc{} {}
		MouseEvent(int2 loc): State(EMouseInputState::NUM), Button(-1), Loc(loc) {}
		MouseEvent(EMouseInputState state, int8 button, int2 loc) : State(state), Button(button), Loc(loc) {}
		EMouseInputState GetState()const { return State; }
		int8 GetButton()const { return Button; }
		int2 GetPosition()const { return Loc; }
	private:
		EMouseInputState State;
		int8 Button;
		int2 Loc;
	};

	class MouseRawEvent
	{
	public:
		MouseRawEvent() :Vel{} {}
		MouseRawEvent(int2 vel) :Vel(vel) {}
		int2 GetVelocity()const { return Vel; }
	private:
		int2 Vel;
	};
public:
	// Inherited via ISubsystem
	virtual void OnPreTick(float dt) override;
	virtual void OnTick(float dt) override;
	virtual void OnInit() override;
	virtual void OnDestroy() override;
public:
	static void EnableAutoRepeat() { s_Input->bAutoRepeatEnable = true; }
	static void DisableAutoRepeat() { s_Input->bAutoRepeatEnable = false; }
	static bool IsAutoRepeat() { return s_Input->bAutoRepeatEnable; }
public:
	static bool IsKeyDown(EKeyCode keyCode) noexcept;
	static bool IsKeyUp(EKeyCode keyCode) noexcept;
	static bool IsSysKeyDown(ESystemKeyCode keyCode) noexcept;
	static bool IsSysKeyUp(ESystemKeyCode keyCode) noexcept;
	static bool IsMouseButtonDown(EMouseButton button) noexcept;
	static bool IsMouseButtonUp(EMouseButton button) noexcept;

	static void BindKeyCharEvent(void(*p)(tchar));
	static void UnbindKeyCharEvent(void(*p)(tchar));
	static bool IsKeyCharEventBound(void(*p)(tchar));

	static void BindKeyInputEvent(EKeyCode keyCode, EKeyInputState keyState, void (*p)());
	static void UnbindKeyInputEvent(EKeyCode keyCode, EKeyInputState keyState, void (*p)());
	static bool IsKeyInputEventBound(EKeyCode keyCode, EKeyInputState keyState, void (*p)());

	static void BindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2));
	static void UnbindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2));
	static bool IsMouseEventBound(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2));

	static void BindMouseMoveEvent(void(*p)(int2));
	static void UnbindMouseMoveEvent(void(*p)(int2));
	static bool IsMouseMoveEventBound(void(*p)(int2));

	static void BindMouseWheelEvent(void(*p)(float));
	static void UnbindMouseWheelEvent(void(*p)(float));
	static bool IsMouseWheelEventBound(void(*p)(float));

	static void BindMouseRawMoveEvent(void(*p)(int2));
	static void UnbindMouseRawMoveEvent(void(*p)(int2));
	static bool IsMouseRawMoveEventBound(void(*p)(int2));

private:
	static void OnKeyInput(EKeyInputState state, EKeyCode keyCode)noexcept;
	static void OnChar(tchar character, bool bIsRepeat)noexcept;
	static void OnMouseEvent(EMouseInputState state, int8 button);
	static void OnMouseMove(int x, int y);
	static void OnMouseRawMove(int x, int y);
	static void OnMouseWheel(float delta);
private:
	template<typename T>
	void TrimEventQueue(std::queue<T>& queue)
	{
		while (queue.size() > MAX_EVENT_QUEUE_SIZE) queue.pop();
	}
	void ProcessKeyboard();
	void ProcessMouse();
private:
	KeyCharEventDelegate OnKeyCharEvent;
	KeyInputEventDelegate OnKeyInputEvent[(uint8)EKeyInputState::NUM][(uint8)EKeyCode::NUM_KEYCODE];
	MouseInputEventDelegate OnMouseButtonEvent[(uint8)EMouseInputState::NUM][(uint8)EMouseButton::Invalid];
	MouseWheelEventDelegate OnMouseWheelEvent;
	MouseInputEventDelegate OnMouseMoveEvent;
	MouseInputEventDelegate OnMouseRawMoveEvent;
private:
	std::queue<KeyboardEvent> KeyboardEventQueue;
	std::queue<tchar> KeyCharEventQueue;
	std::queue<MouseEvent> MouseEventQueue;
	std::queue<MouseRawEvent> MouseRawEventQueue;
	static constexpr size_t MAX_EVENT_QUEUE_SIZE = 16;
	int2 PrevMousePosition;
	int2 MousePosition;
	float MouseWheelDelta;

	uint8 bAutoRepeatEnable : 1;
	uint8 bEnableMouseRawInput : 1;
	static Input* s_Input;
};

