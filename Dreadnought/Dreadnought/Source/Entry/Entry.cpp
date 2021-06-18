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


//TMulticastDelegate<int, float> f;
//f.Bind([](int a, float b) {wprintf_s(L"%d  %f  ", a, b); });
//f.Broadcast(1, 2.5f);

/*
template<typename... Vars>
class TTest
{

};

template<typename Var, typename... Vars>
class TTest<Var, Vars...> : public TTest<Vars...>
{
public:
	TTest(Var v, Vars... vs) : TTest<Vars...>(vs...), var(v) {}
	Var var;
};

TTest<float, int, char> test(1.4f, 3, 'v');

template<size_t, typename> struct elem_type_holder;

template<typename T, typename... Ts>
struct elem_type_holder<0, TTest<T, Ts...>>
{
	typedef T type;
};

template<size_t k, typename T, typename... Ts>
struct elem_type_holder<k, TTest<T, Ts...>>
{
	typedef typename elem_type_holder<k - 1, TTest<Ts...>>::type type;
};

template<size_t k, typename... Ts>
typename std::enable_if<k == 0, typename elem_type_holder<0, TTest<Ts...>>::type&>::type
get(TTest<Ts...>& t) { return t.var; }

template<size_t k, typename T, typename... Ts>
typename std::enable_if<k != 0, typename elem_type_holder<k, TTest<T, Ts...>>::type&>::type
get(TTest<T, Ts...>& t) {
	TTest<Ts...>& base = t;
	return get<k - 1>(base);
}
#include <iostream>
	get<1>(test);
*/
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Engine::GetInstance()->Init();
	
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
	}
	catch (const HrException& e)
	{
		MessageBox(nullptr, e.GetExceptionMessage(), e.GetType(), 0);
	}
	catch (const MsgException& e)
	{
		MessageBox(nullptr, e.GetExceptionMessage(), e.GetType(), 0);
	}
	catch (...)
	{
		MessageBox(nullptr, L"Unknown error", L"Unknown Error", 0);
	}

	FreeConsole();
	Engine::GetInstance()->OnDestroy();
	return 0;
}
