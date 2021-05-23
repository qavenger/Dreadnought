#include "pch.h"
#include "Input.h"

POINT Input::m_prevMousePos;
POINT Input::m_mousePos;
HWND Input::m_hWnd;
KeyCharEvent Input::OnKeyCharEvent;
KeyInputEvent Input::OnKeyInputEvent[(uint8)EKeyInputState::NUM][(unsigned long long)Input::EKeyCode::NUM_KEYCODE];
MouseEvent Input::OnMouseButtonEvent[(int)EMouseInputState::NUM][(int)EMouseButton::Invalid];
MouseEvent Input::OnMouseMoveEvent;
EMouseInputState Input::MouseInputState[(int)Input::EMouseButton::Invalid] = {EMouseInputState::UP,EMouseInputState::UP,EMouseInputState::UP};

void Input::Start(HWND hWnd)
{
	m_hWnd = hWnd;
}

void Input::BindKeyCharEvent(void (*p)(TCHAR, bool))
{
	OnKeyCharEvent.Bind(p);
}

void Input::UnBindKeyCharEvent(void(*p)(TCHAR, bool))
{
	OnKeyCharEvent.UnBind(p);
}

bool Input::IsBoundKeyCharEvent(void(*p)(TCHAR, bool))
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

bool Input::OnKeyInput(EKeyCode keyCode, EKeyInputState keyState)
{
	OnKeyInputEvent[(uint8)keyState][(uint64)keyCode].Broadcast();
	return true;
}

void Input::BindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2))
{
	OnMouseButtonEvent[(int)mouseState][(int)mouseButton].Bind(p);
}

void Input::UnBindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2))
{
	OnMouseButtonEvent[(int)mouseState][(int)mouseButton].UnBind(p);
}

bool Input::OnMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, int x, int y)
{
	m_prevMousePos = m_mousePos;
	m_mousePos.x = x;
	m_mousePos.y = y;
	OnMouseButtonEvent[(int)mouseState][(int)mouseButton].Broadcast({x,y});
	MouseInputState[(int)mouseButton] = mouseState;
	return true;
}

void Input::BindMouseMoveEvent(void(*p)(int2))
{
	OnMouseMoveEvent.Bind(p);
}

void Input::UnBindMouseMoveEvent(void(*p)(int2))
{
	OnMouseMoveEvent.UnBind(p);
}

bool Input::OnMouseMove(int x, int y)
{
	OnMouseMoveEvent.Broadcast({ x,y });
	return true;
}

bool Input::OnKeyChar(const TCHAR character, bool bIsRepeat)
{
	if (character == '\x1b')return false;
	OnKeyCharEvent.Broadcast(character, bIsRepeat);
	return true;
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

void Input::UpdateMouse( Input::EMouseButton keyCode, EMouseInputState mouseState, int x, int y)
{
	m_prevMousePos = m_mousePos;
	m_mousePos.x = x;
	m_mousePos.y = y;
}
