#include <pch.h>
#include "Window/Window.h"
#include "Graphics/Graphics.h"

static std::wstring InputBuffer;
void KeyChar(TCHAR character, bool isRepeat)
{
	if (character == '`')return;
	bool isBackspace = character == '\b';
	bool isReturn = character == '\r';
	bool needBroadcast = false;
	bool needProcessBuffer = false;

	if (InputBuffer.length() > 0 && isBackspace)
	{
		InputBuffer.pop_back();
		needBroadcast = true;
	}
	else if (isReturn && InputBuffer.length() > 0)
	{
		needBroadcast = true;
		needProcessBuffer = true;
	}
	else if (InputBuffer.length() < 63)
	{
		InputBuffer.push_back(character);
		needBroadcast = true;
	}

	if (needBroadcast)
	{
		if (isReturn)
			wprintf_s(L"\r\n");
		else if (isBackspace)
			wprintf_s(L"\b \b");
		else
			wprintf_s(&character);
	}
	if (needProcessBuffer)
	{
		wprintf_s(L"Echo: %s\n", InputBuffer.data());
		InputBuffer.clear();
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Window Wnd;
	Wnd.SetTitle(L"Engine");
	Wnd.SetWindowMode(EWindowMode::WINDOW);
	Wnd.Init_Wnd();
	Input::Start(Wnd.GetHandle());
	MSG msg = {};
	if (!Graphics::GetInstance()->Init(&Wnd))
	{
		return -1;
	}
	Input::BindKeyInput(Input::EKeyCode::Escape, EKeyInputState::RELEASED, []() {PostQuitMessage(0); });
	Input::BindKeyInput(Input::EKeyCode::Escape, EKeyInputState::PRESSED, []() {PrintDebugMessage(L"Escape Pressed"); });
	Input::BindKeyInput(Input::EKeyCode::Tilde, EKeyInputState::RELEASED, []() {
		if (Input::IsBoundKeyCharEvent(KeyChar)) Input::UnBindKeyCharEvent(KeyChar);
		else Input::BindKeyCharEvent(KeyChar); 
		});


	Input::BindMouseEvent(Input::EMouseButton::Left, EMouseInputState::DBLCLK, [](int2 pos) {
		wprintf_s(L"Left Double Clicked At: {%d, %d}\n", pos.x, pos.y);
		});
	Input::BindMouseEvent(Input::EMouseButton::Left, EMouseInputState::UP, [](int2 pos) {
		wprintf_s(L"Left Up At: {%d, %d}\n", pos.x, pos.y);
		});
	Input::BindMouseEvent(Input::EMouseButton::Left, EMouseInputState::DOWN, [](int2 pos) {
		wprintf_s(L"Left Down At: {%d, %d}\n", pos.x, pos.y);
		});
	Input::BindMouseMoveEvent([](int2 pos) {
			if(Input::IsMouseButtonDown(Input::EMouseButton::Left))
				wprintf_s(L"Left Move At: {%d, %d}\n", pos.x, pos.y);
		});

	while (msg.message != WM_QUIT)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	FreeConsole();
	return 0;
}