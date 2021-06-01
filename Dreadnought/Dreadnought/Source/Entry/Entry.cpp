#include <pch.h>
#include "Core/Engine.h"
#include "Window/Window.h"
#include "Renderer/SceneRenderer.h"

static std::wstring InputBuffer;
void KeyChar(TCHAR character, bool isRepeat);

void ProcessInputBuffer()
{
	std::vector<std::wstring> token;
	size_t head = 0;
	size_t tail = 0;
	while (tail < InputBuffer.size())
	{
		tail = InputBuffer.find(L' ', head);
		token.push_back(InputBuffer.substr(head, tail - head));
		head = tail + 1;
	}
	wprintf_s(L"Echo: %d\n", (int)token.size());
	std::vector<int> params;
	params.reserve(token.size());
	for (auto s : token)
	{
		params.push_back(_wtoi(s.c_str()));
	}

	if (params.size() == 2)
	{
		Window::GetInstance()->SetDimension(params[0], params[1]);
	}
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
		{
			wprintf_s(L"\r\n");
		}
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

//#include <iostream>

//template<typename T>
//void Print(T&& param)
//{
//	std::cout << std::forward<T>(param);
//}
//
//template<typename A, typename ...B>
//void Print(A&& param, B&&... rest)
//{
//	Print(std::forward<A>(param));
//	Print(std::forward<B>(rest)...);
//}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Engine::GetInstance()->Init();
	}
	catch (HrException& e)
	{
		printf("%s", e.what());
	}
	MSG msg = {};
	Input::BindKeyInput(Input::EKeyCode::Escape, EKeyInputState::RELEASED, []() {PostQuitMessage(0); });
	Input::BindKeyInput(Input::EKeyCode::Escape, EKeyInputState::PRESSED, []() {PrintDebugMessage(L"Escape Pressed"); });
	
	while (msg.message != WM_QUIT)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Engine::GetInstance()->Run();
	}

	FreeConsole();
	Engine::GetInstance()->OnDestroy();
	return 0;
}