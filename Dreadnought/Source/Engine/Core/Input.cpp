#include <stdafx.h>
#include "Input.h"

Input* Input::s_Input = nullptr;

void Input::OnPreTick(float dt)
{
	MouseWheelDelta = 0;
	PrevMousePosition = MousePosition;
}

void Input::OnTick(float dt)
{
	ProcessKeyboard();
	ProcessMouse();
}

void Input::OnInit()
{
	s_Input = (Input*)ISubsystem::GetSubsystem("Input");
	ThrowIfFalse(s_Input != nullptr, "Failed to create Input subsystem");
}

void Input::OnDestroy()
{
	
}

bool Input::IsKeyDown(EKeyCode keyCode) noexcept
{
	return (GetKeyState((int)keyCode) & 0x80) != 0;
}

bool Input::IsKeyUp(EKeyCode keyCode) noexcept
{
	return (GetKeyState((int)keyCode) & 0x80) == 0;
}

bool Input::IsSysKeyDown(ESystemKeyCode keyCode) noexcept
{
	return (GetKeyState((int)keyCode) & 0x80) != 0;
}

bool Input::IsSysKeyUp(ESystemKeyCode keyCode) noexcept
{
	return (GetKeyState((int)keyCode) & 0x80) == 0;
}

bool Input::IsMouseButtonDown(EMouseButton button)  noexcept
{
	return (GetKeyState((int)button) & 0x80) != 0;
}

bool Input::IsMouseButtonUp(EMouseButton button)  noexcept
{
	return (GetKeyState((int)button) & 0x80) == 0;
}

void Input::BindKeyCharEvent(void(*p)(tchar))
{
	s_Input->OnKeyCharEvent.Bind(p);
}

void Input::UnbindKeyCharEvent(void(*p)(tchar))
{
	s_Input->OnKeyCharEvent.Unbind(p);
}

bool Input::IsKeyCharEventBound(void(*p)(tchar))
{
	return s_Input->OnKeyCharEvent.IsBound(p);
}

void Input::BindKeyInputEvent(EKeyCode keyCode, EKeyInputState keyState, void(*p)())
{
	s_Input->OnKeyInputEvent[(uint8)keyState][(uint8)keyCode].Bind(p);
}

void Input::UnbindKeyInputEvent(EKeyCode keyCode, EKeyInputState keyState, void(*p)())
{
	s_Input->OnKeyInputEvent[(uint8)keyState][(uint8)keyCode].Unbind(p);
}

bool Input::IsKeyInputEventBound(EKeyCode keyCode, EKeyInputState keyState, void(*p)()) 
{
	return s_Input->OnKeyInputEvent[(uint8)keyState][(uint8)keyCode].IsBound(p);
}

void Input::BindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2))
{
	int8 btnIdx = -1;
	switch (mouseButton)
	{
	case EMouseButton::Left:
		btnIdx = 0;
		break;
	case EMouseButton::Middle:
		btnIdx = 1;
		break;
	case EMouseButton::Right:
		btnIdx = 2;
		break;
	}
	if (btnIdx > -1 && btnIdx < (int8)EMouseButton::Invalid)
	{
		s_Input->OnMouseButtonEvent[(uint8)mouseState][btnIdx].Bind(p);
	}
}

void Input::UnbindMouseEvent(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2))
{
	int8 btnIdx = -1;
	switch (mouseButton)
	{
	case EMouseButton::Left:
		btnIdx = 0;
		break;
	case EMouseButton::Middle:
		btnIdx = 1;
		break;
	case EMouseButton::Right:
		btnIdx = 2;
		break;
	}
	if (btnIdx > -1 && btnIdx < (int8)EMouseButton::Invalid)
	{
		s_Input->OnMouseButtonEvent[(uint8)mouseState][btnIdx].Unbind(p);
	}
}

bool Input::IsMouseEventBound(EMouseButton mouseButton, EMouseInputState mouseState, void(*p)(int2)) 
{
	int8 btnIdx = -1;
	switch (mouseButton)
	{
	case EMouseButton::Left:
		btnIdx = 0;
		break;
	case EMouseButton::Middle:
		btnIdx = 1;
		break;
	case EMouseButton::Right:
		btnIdx = 2;
		break;
	}
	return (btnIdx != -1) && s_Input->OnMouseButtonEvent[(uint8)mouseState][btnIdx].IsBound(p);
}

void Input::BindMouseMoveEvent(void(*p)(int2))
{
	s_Input->OnMouseMoveEvent.Bind(p);
}

void Input::UnbindMouseMoveEvent(void(*p)(int2))
{
	s_Input->OnMouseMoveEvent.Unbind(p);
}

bool Input::IsMouseMoveEventBound(void(*p)(int2)) 
{
	return s_Input->OnMouseMoveEvent.IsBound(p);
}

void Input::BindMouseWheelEvent(void(*p)(float))
{
	s_Input->OnMouseWheelEvent.Bind(p);
}

void Input::UnbindMouseWheelEvent(void(*p)(float))
{
	s_Input->OnMouseWheelEvent.Unbind(p);
}

bool Input::IsMouseWheelEventBound(void(*p)(float)) 
{
	return s_Input->OnMouseWheelEvent.IsBound(p);
}

void Input::BindMouseRawMoveEvent(void(*p)(int2))
{
	s_Input->OnMouseRawMoveEvent.Bind(p);
}

void Input::UnbindMouseRawMoveEvent(void(*p)(int2))
{
	s_Input->OnMouseRawMoveEvent.Unbind(p);
}

bool Input::IsMouseRawMoveEventBound(void(*p)(int2)) 
{
	return s_Input->OnMouseRawMoveEvent.IsBound(p);;
}

void Input::OnKeyInput(EKeyInputState state, EKeyCode keyCode) noexcept
{
	s_Input->KeyboardEventQueue.push(KeyboardEvent(state, keyCode));
}

void Input::OnChar(tchar character, bool bIsRepeat) noexcept
{
	if(!bIsRepeat || s_Input->bAutoRepeatEnable)
		s_Input->KeyCharEventQueue.push(character);
}

void Input::OnMouseEvent(EMouseInputState state, int8 button)
{
	s_Input->MouseEventQueue.push(MouseEvent(state, button, s_Input->MousePosition));
}

void Input::OnMouseMove(int x, int y)
{
	s_Input->MouseEventQueue.push(MouseEvent(EMouseInputState::MOVE, -1, { x, y }));
	s_Input->MousePosition = { x,y };
}

void Input::OnMouseRawMove(int x, int y)
{
	if (s_Input->bEnableMouseRawInput)
	{
		s_Input->MouseRawEventQueue.push(MouseRawEvent({ x,y }));
	}
}

void Input::OnMouseWheel(float delta)
{
	s_Input->MouseWheelDelta = delta;
}

void Input::ProcessKeyboard()
{
	while (!KeyCharEventQueue.empty())
	{
		auto& e = KeyCharEventQueue.front();
		OnKeyCharEvent.Broadcast(e);
		KeyCharEventQueue.pop();
	}
	while (!KeyboardEventQueue.empty())
	{
		auto& e = KeyboardEventQueue.front();
		OnKeyInputEvent[(uint8)e.GetState()][(uint8)e.GetKeyCode()].Broadcast();
		KeyboardEventQueue.pop();
	}
}

void Input::ProcessMouse()
{
	while (!MouseEventQueue.empty())
	{
		auto& e = MouseEventQueue.front();
		if (e.GetState() == EMouseInputState::MOVE)
		{
			OnMouseMoveEvent.Broadcast(e.GetPosition());
		}
		else
		{
			OnMouseButtonEvent[(uint8)e.GetState()][(uint8)e.GetButton()].Broadcast(e.GetPosition());
		}
		MouseEventQueue.pop();
	}

	while (!MouseRawEventQueue.empty())
	{
		auto& e = MouseRawEventQueue.front();
		OnMouseRawMoveEvent.Broadcast(e.GetVelocity());
		MouseRawEventQueue.pop();
	}

	if (MouseWheelDelta != 0)
	{
		OnMouseWheelEvent.Broadcast(MouseWheelDelta);
	}
}
