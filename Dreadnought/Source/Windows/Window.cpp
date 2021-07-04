#include "stdafx.h"
#include "Window.h"
#include <Engine.h>

static LRESULT CALLBACK WndProcSetup(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam);

WindowPool* Window::SubSystem = nullptr;
WindowPool* WindowPool::s_WindowPool = nullptr;
const Window* WindowPool::GetWindowFromHandle(HWND hWnd)const
{
	auto itr = WindowLookup.find(hWnd);
	if (itr != WindowLookup.end())
	{
		return &Windows[itr->second];
	}
	return nullptr;
}

const Window* WindowPool::GetWindowFromIndex(int16 index) const
{
	assert(index >= 0 && index < Windows.size());
	return &Windows[index];
}

Window* WindowPool::GetWindowFromHandle(HWND hWnd)
{
	auto itr = WindowLookup.find(hWnd);
	if (itr != WindowLookup.end())
	{
		return &Windows[itr->second];
	}
	return nullptr;
}

int16 WindowPool::GetWindowIndex(HWND hWnd) const
{
	auto itr = WindowLookup.find(hWnd);
	if (itr == WindowLookup.end()) return -1;
	return itr->second;
}

Window* WindowPool::GetWindowFromIndex(int16 index)
{
	assert(index >= 0 && index < Windows.size());
	return &Windows[index];
}

void WindowPool::OnInit()
{
	hInstance = GetModuleHandle(nullptr);
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS | CS_OWNDC;
	wc.hInstance = hInstance;
	wc.lpszClassName = WndClassName;
	wc.lpfnWndProc = WndProcSetup;

	FatalAndThrowLastError(RegisterClassEx(&wc));

	RAWINPUTDEVICE rid = {};
	rid.usUsage = 0x02;
	rid.usUsagePage = 0x01;
	ThrowLastError(RegisterRawInputDevices(&rid, 1, sizeof(rid)));

	Windows.reserve(MAX_NUM_WINDOWS);
	s_WindowPool = this;

	Windows.push_back(std::move(Window()));
	WindowLookup[Windows[0].hWnd] = 0;
}

void WindowPool::OnDestroy()
{
	for (auto itr = WindowLookup.begin(); itr != WindowLookup.end(); ++itr)
	{
		if (itr->first)
		{
			itr = DestroyWindow(itr->first);
			if (itr == WindowLookup.end())break;
		}
	}

	Windows.clear();
	WindowLookup.clear();
	EmptySlots.clear();
	ImGui_ImplWin32_Shutdown();
}

void WindowPool::PendingKill(int16 index)
{
	Windows[index].OnDestroy();
	PendingKillList.push_back(index);
}

void WindowPool::OnPostTick(float dt)
{
	if (!PendingKillList.empty())
	{
		for (size_t i = 0; i < PendingKillList.size(); ++i)
		{
			Windows[PendingKillList[i]].Destroy();
		}
		PendingKillList.clear();
	}
}

HWND WindowPool::GetMainWindow()
{
	//FatalIfFalse(false, L"Test");
	FatalIfFalse(!s_WindowPool->Windows.empty() && s_WindowPool->Windows[0].hWnd != nullptr, "No Window Presented");
	return s_WindowPool->Windows[0].GetWindowHandle();
}

WindowPool::~WindowPool()
{
	UnregisterClass(WndClassName, hInstance);
}

LRESULT CALLBACK WndProcSetup(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	
	case WM_DESTROY:
	{
		WindowPool* wndPool = (WindowPool*)ISubsystem::GetSubsystem("WindowPool");
		if (wndPool)
		{
			Window* wnd = wndPool->GetWindowFromHandle(hWnd);
				
			if (!wnd->GetParent())
			{
				PostQuitMessage(0);
				return 0;
			}
			else
			{
				wndPool->DestroyWindow(hWnd);
			}
		}
	}
	break;
	}
	return gEngine->WindowProc(hWnd, msg, wParam, lParam);
}

Window::Window():
	Super(),
	hWnd(nullptr),
	Parent(-1),
	WndRect{},
	Title("Dreadought")
{
	if (!SubSystem) SubSystem = (WindowPool*)ISubsystem::GetSubsystem("WindowPool");
	hWnd = CreateWindowEx(
		WS_EX_ACCEPTFILES
		, SubSystem->GetWndClassName()
		, Title.c_str()
		, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT, CW_USEDEFAULT
		, CW_USEDEFAULT, CW_USEDEFAULT
		, nullptr
		, nullptr
		, SubSystem->GetWndInstanceHandle()
		, gEngine.get());

	FatalIfFalse(hWnd != nullptr, "Created Window Failed");
	GetClientRect(hWnd, &WndRect);
	ShowWindow(hWnd, SW_SHOW);
}

Window::Window(const tchar* title, int16 parent) :Super(),Parent(-1),hWnd(nullptr), WndRect{},Title(title)
{
	assert(SubSystem);
	hWnd = CreateWindow(
		  SubSystem->GetWndClassName()
		, title
		, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT, CW_USEDEFAULT
		, CW_USEDEFAULT, CW_USEDEFAULT
		, nullptr
		, nullptr
		, SubSystem->GetWndInstanceHandle()
		, nullptr);
	GetClientRect(hWnd, &WndRect);
	ShowWindow(hWnd, SW_SHOW);
}

Window* Window::GetParent()
{
	if (SubSystem == nullptr || Parent < 0) return nullptr;
	return SubSystem->GetWindowFromIndex(Parent);
}

HWND Window::GetParentWindowHandle()
{
	if (Parent < 0) return nullptr;
	WindowPool* wndPool = (WindowPool*)ISubsystem::GetSubsystem("WindowPool");
	if (!wndPool) return nullptr;
	return wndPool->GetWindowFromIndex(Parent)->hWnd;
}

HWND Window::GetWindowHandle()
{
	return hWnd;
}

void Window::Resize(uint32 w, uint32 h)
{
	GetClientRect(hWnd, &WndRect);
}

void Window::Destroy()
{
	if (hWnd)
	{
		if (Parent > -1)
		{
			Window* parent = SubSystem->GetWindowFromIndex(Parent);
			parent->Children.erase(SubSystem->GetWindowIndex(hWnd));
		}
		DestroyWindow(hWnd);
		hWnd = nullptr;
	}
}

void Window::OnDestroy()
{
	for (auto i : Children)
	{
		SubSystem->PendingKill(i);
	}
	WndRect = {};
	Children.clear();
	Title.clear();
	Super::OnDestroy();
}

//void Window::Resize(uint32 w, uint32 h)
//{
//	RECT rect = {WndRect.left, WndRect.right, WndRect.left+w, WndRect.right+h};
//	if (AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE))
//	{
//		WndRect = rect;
//	}
//
//	SetWindowPos(hWnd,  HWND_TOPMOST, SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOZORDER);
//}

Window* WindowPool::New(const tchar* title, Window* parent)
{
	int16 slot = -1;
	if (Windows.size() >= MAX_NUM_WINDOWS) return nullptr;
	if (EmptySlots.empty())
	{
		slot = (int16)Windows.size();
		Windows.push_back(std::move(Window(title)));
	}
	else
	{ 
		slot = EmptySlots.back();
		EmptySlots.pop_back();
		Windows[slot] = std::move(Window(title));
	}
	WindowLookup[Windows[slot].hWnd] = slot;
	
	
	if (parent)
	{
		auto itr = WindowLookup.find(parent->GetWindowHandle());
		if (itr !=  WindowLookup.end())
		{
			auto parentSlot = itr->second;
			Windows[slot].Parent = parentSlot;
			Windows[parentSlot].Children.insert(slot);
		}
	}
	else
	{
		Windows[slot].Parent = 0;
		Windows[0].Children.insert(slot);
	}

	return &Windows[slot];
}

std::map<HWND, int16>::iterator WindowPool::DestroyWindow(HWND wndToDestroy)
{
	auto itr = WindowLookup.find(wndToDestroy);
	if (itr != WindowLookup.end())
	{
		auto slot = itr->second;
		Windows[slot].OnDestroy();
		Windows[slot].hWnd = nullptr;
		Windows[slot].Parent = -1;
		Windows[slot].WndRect = {};
		Windows[slot].Children.clear();
		EmptySlots.push_back(slot);
	}
	return WindowLookup.erase(itr);
}

