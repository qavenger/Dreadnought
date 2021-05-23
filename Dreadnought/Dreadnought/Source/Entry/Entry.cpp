#include <pch.h>
#include "Core/Engine.h"
#include "Window/Window.h"
#include "Graphics/Graphics.h"

static std::wstring InputBuffer;
void ProcessInputBuffer()
{
	wprintf_s(L"Echo: %s\n", InputBuffer.data());
}

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
		ProcessInputBuffer();
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
	if (!Engine::GetInstance()->Init(&Wnd))
	{
		return -1;
	}
	MSG msg = {};
	Input::BindKeyInput(Input::EKeyCode::Escape, EKeyInputState::RELEASED, []() {PostQuitMessage(0); });
	Input::BindKeyInput(Input::EKeyCode::Escape, EKeyInputState::PRESSED, []() {PrintDebugMessage(L"Escape Pressed"); });
	Input::BindKeyInput(Input::EKeyCode::Tilde, EKeyInputState::RELEASED, []() {
		if (Input::IsBoundKeyCharEvent(KeyChar)) Input::UnBindKeyCharEvent(KeyChar);
		else Input::BindKeyCharEvent(KeyChar); 
		});
	Input::BindKeyInput(Input::EKeyCode::R, EKeyInputState::PRESSED, []() {Graphics::ReportLiveObject(); });

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