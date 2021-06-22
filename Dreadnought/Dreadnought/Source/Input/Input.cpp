#include "pch.h"
#include "Input.h"

POINT Input::m_prevMousePos;
POINT Input::m_mousePos;
HWND Input::m_hWnd;
KeyCharEvent Input::OnKeyCharEvent;
KeyInputEvent Input::OnKeyInputEvent[(uint8)EKeyInputState::NUM][(unsigned long long)Input::EKeyCode::NUM_KEYCODE];
MouseInputEvent Input::OnMouseButtonEvent[(int)EMouseInputState::NUM][(int)EMouseButton::Invalid];
MouseInputEvent Input::OnMouseMoveEvent;
MouseInputEvent Input::OnMouseRawMoveEvent;
MouseWheelEvent Input::OnMouseWheelEvent;
EMouseInputState Input::MouseInputState[(int)Input::EMouseButton::Invalid] = {EMouseInputState::UP,EMouseInputState::UP,EMouseInputState::UP};
std::vector<char> Input::RawInputBuffer;
bool Input::bAutoRepeatEnable = true;
bool Input::bEnableRawInput = true;
float Input::MouseWheelDelta = 0;
std::queue<TCHAR> Input::CharBuffer;
std::queue<Input::KeyEvent> Input::KeyBuffer;
std::queue<Input::MouseEvent> Input::MouseBuffer;
std::queue<Input::MouseRawEvent> Input::MouseRawBuffer;
void Input::Start(HWND hWnd)
{
	m_hWnd = hWnd;
}

void Input::BindKeyCharEvent(void (*p)(TCHAR))
{
	OnKeyCharEvent.Bind(p);
}

void Input::UnBindKeyCharEvent(void(*p)(TCHAR))
{
	OnKeyCharEvent.UnBind(p);
}

bool Input::IsBoundKeyCharEvent(void(*p)(TCHAR))
{
	return OnKeyCharEvent.IsBound(p);
}

void Input::BindKeyInput(EKeyCode keyCode, EKeyInputState keyState, void(*p)())
{
	OnKeyInputEvent[(uint8)keyState][(uint64)keyCode].Bind(p);
}

void Input::UnBindKeyInput(EKeyCode keyCode, EKeyInputState keyState, void(*p)())
{
	OnKeyInputEvent[(uint8)keyState][(uint64)keyCode].UnBind(p);
}

void Input::OnKeyInput(EKeyCode keyCode, EKeyInputState keyState)
{
	KeyBuffer.push(KeyEvent(keyState, keyCode));
	//OnKeyInputEvent[(uint8)keyState][(uint64)keyCode].Broadcast();
}

void Input::BindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2))
{
	OnMouseButtonEvent[(int)mouseState][(int)mouseButton].Bind(p);
}

void Input::UnBindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2))
{
	OnMouseButtonEvent[(int)mouseState][(int)mouseButton].UnBind(p);
}

void Input::OnMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState)
{
	//OnMouseButtonEvent[(int)mouseState][(int)mouseButton].Broadcast({x,y});
	MouseInputState[(int)mouseButton] = mouseState;
	MouseBuffer.push(MouseEvent(mouseState, mouseButton, { m_mousePos.x, m_mousePos.y }));
}

void Input::BindMouseMoveEvent(void(*p)(int2))
{
	OnMouseMoveEvent.Bind(p);
}

void Input::UnBindMouseMoveEvent(void(*p)(int2))
{
	OnMouseMoveEvent.UnBind(p);
}

void Input::OnMouseMove(int x, int y)
{
	//OnMouseMoveEvent.Broadcast({ x,y });
	MouseBuffer.push(MouseEvent({x, y}));
	m_mousePos.x = x;
	m_mousePos.y = y;
}

void Input::ProcessKeyborad()
{
	while (!KeyBuffer.empty())
	{
		auto& e = KeyBuffer.front();
		OnKeyInputEvent[(int)e.GetState()][e.GetKeyCode()].Broadcast();
		KeyBuffer.pop();
	}
	while (!CharBuffer.empty())
	{
		OnKeyCharEvent.Broadcast(CharBuffer.front());
		CharBuffer.pop();
	}
}

void Input::ProcessMouse()
{
	while (!MouseBuffer.empty())
	{
		auto& e = MouseBuffer.front();
		if (e.GetState() == EMouseInputState::MOVE)
		{
			OnMouseMoveEvent.Broadcast(e.GetPosition());
		}
		else
		{
			OnMouseButtonEvent[(int)e.GetState()][(int)e.GetButton()].Broadcast(e.GetPosition());
		}
		MouseBuffer.pop();
	}
	while (!MouseRawBuffer.empty())
	{
		auto& e = MouseRawBuffer.front();
		OnMouseRawMoveEvent.Broadcast(e.GetVelocity());
		MouseRawBuffer.pop();
	}
	if (MouseWheelDelta != 0)
	{
		OnMouseWheelEvent.Broadcast(MouseWheelDelta);
	}
}

void Input::PreTick()
{
	MouseWheelDelta = 0;
	m_prevMousePos = m_mousePos;
}

void Input::Tick()
{
	ProcessKeyborad();
	ProcessMouse();
}

void Input::BindMouseRawMoveEvent(void(*p)(int2))
{
	OnMouseRawMoveEvent.Bind(p);
}

void Input::UnBindMouseRawMoveEvent(void(*p)(int2))
{
	OnMouseRawMoveEvent.UnBind(p);
}

void Input::BindMouseWheelEvent(void(*p)(float))
{
	OnMouseWheelEvent.Bind(p);
}

void Input::UnBindMouseWheelEvent(void(*p)(float))
{
	OnMouseWheelEvent.UnBind(p);
}

void Input::OnMouseRawMove(int x, int y)
{
	//RawMoveVel = { x,y };
	//OnMouseRawMoveEvent.Broadcast({ x,y });
	if (bEnableRawInput)
	{
		MouseRawBuffer.push(MouseRawEvent({ x, y }));
	}
}

void Input::OnMouseWheel(float delta)
{
	MouseWheelDelta = delta;
}

void Input::OnKeyChar(const TCHAR character, bool bIsRepeat)
{
	//OnKeyCharEvent.Broadcast(character, bIsRepeat);
	if (bIsRepeat && !bAutoRepeatEnable)
	{
		return;
	}
	CharBuffer.push(character);
}

bool Input::IsKeyDown(EKeyCode keyCode)
{
	return (GetKeyState((int)keyCode) & 0x80) != 0;
}

bool Input::IsKeyUp(EKeyCode keyCode)
{
	return (GetKeyState((int)keyCode) & 0x80) == 0;
}

int2 Input::GetMousePosition()
{
	POINT pos = m_mousePos;
	ScreenToClient(m_hWnd, &pos);
	return int2(pos.x, pos.y);
}

int2 Input::GetScreenMousePosition()
{
	return int2(m_mousePos.x, m_mousePos.y);
}

int2 Input::GetMouseRelativePosition()
{
	return int2(
		m_mousePos.x - m_prevMousePos.x,
		m_mousePos.y - m_prevMousePos.y
	);
}

void Input::ClearKeyEvent()
{
	KeyBuffer = std::queue<KeyEvent>();
}

void Input::ClearCharEvent()
{
	CharBuffer = std::queue<TCHAR>();
}

void Input::ClearKeyboardEvents()
{
	ClearKeyEvent();
	ClearCharEvent();
}

void Input::EnableAutoRepeat()
{
	bAutoRepeatEnable = true;
}

void Input::DisableAutoRepeat()
{
	bAutoRepeatEnable = false;
}

bool Input::IsAutoRepeatEnabled()
{
	return bAutoRepeatEnable;
}

Input::KeyEvent::KeyEvent(EKeyInputState state, EKeyCode keyCode)
	:
	State(state),
	KeyCode((unsigned char)keyCode)
{
}

Input::MouseEvent::MouseEvent(EMouseInputState state, EMouseButton button, int2 loc)
	:
	State(state),
	Button(button),
	Loc(loc)
{
}

Input::MouseEvent::MouseEvent(int2 loc)
	:
	State(EMouseInputState::MOVE),
	Button(EMouseButton::Invalid),
	Loc(loc)
{
}
