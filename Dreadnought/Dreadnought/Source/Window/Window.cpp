#include "pch.h"
#include "Window.h"

bool Window::Init_Wnd()
{
#if defined(DEBUG) || defined (_DEBUG)
	AllocConsole();
	ConsoleHandle = GetConsoleWindow();
	FILE* pFile;
	freopen_s(&pFile, "CONOUT$", "w+t", stdout);
	
	freopen_s(&pFile, "CONIN$", "r+t", stdin);
#else
	//AttachConsole(ATTACH_PARENT_PROCESS)
#endif
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
	
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(wc.hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = Title.c_str();
	wc.lpfnWndProc = WndProc;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Register Window Class Failed", L"ERROR", 0);
		return false;
	}

	bool windowed = GetWindowMode() == EWindowMode::WINDOW;
	/*uint16 width, height;
	GetDimension(width, height);
	AspectRatio = (float)width / height;
	int x = fullScreen ? 0 : CW_USEDEFAULT;
	int y = fullScreen ? 0 : CW_USEDEFAULT;*/
	UINT dwStyle = windowed ?   WS_OVERLAPPED | WS_SYSMENU : WS_POPUP;
	WindowHandle = CreateWindow(wc.lpszClassName, Title.c_str(), dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, wc.hInstance, 0);
	
	if (!WindowHandle)
	{
		MessageBox(NULL, L"Create Window Failed", L"ERROR", 0);
		return false;
	}
	return true;
}

LRESULT Window::WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		wprintf_s(L"%u x %u\n", LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		//if (wParam & MK_LBUTTON)
		{
			int x, y;
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			Input::OnMouseMove(x,y);
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	{
		int x, y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		Input::EMouseButton btn = Input::EMouseButton::Invalid;
		bool bIsDblClk = false;
		bool bIsMouseUp = false;
		switch (msg)
		{
		case WM_LBUTTONDOWN:
			btn = Input::EMouseButton::Left;
			break;
		case WM_RBUTTONDOWN:
			btn = Input::EMouseButton::Right;
			break;
		case WM_MBUTTONDOWN:
			btn = Input::EMouseButton::Middle;
			break;
		case WM_LBUTTONUP:
			btn = Input::EMouseButton::Left;
			bIsMouseUp = true;
			break;
		case WM_RBUTTONUP:
			btn = Input::EMouseButton::Right;
			bIsMouseUp = true;
			break;
		case WM_MBUTTONUP:
			btn = Input::EMouseButton::Middle;
			bIsMouseUp = true;
			break;
		case WM_LBUTTONDBLCLK:
			btn = Input::EMouseButton::Left;
			bIsDblClk = true;
			break;
		case WM_RBUTTONDBLCLK:
			btn = Input::EMouseButton::Right;
			bIsDblClk = true;
			break;
		case WM_MBUTTONDBLCLK:
			btn = Input::EMouseButton::Middle;
			bIsDblClk = true;
			break;
		}
		if (bIsDblClk)
		{
			Input::OnMouseEvent(btn, EMouseInputState::DBLCLK, x, y);
		}
		else if (bIsMouseUp)
		{
			Input::OnMouseEvent(btn, EMouseInputState::UP, x, y);
		}
		else
		{
			Input::OnMouseEvent(btn, EMouseInputState::DOWN, x, y);
		}
	}
		break;
	case WM_KEYDOWN:
	{
		if ((lParam & 0xC0000000) == 0)
		{
			Input::OnKeyInput((Input::EKeyCode)(wParam), EKeyInputState::PRESSED);
		}
		break;
	}
	case WM_KEYUP:
	{
		Input::OnKeyInput((Input::EKeyCode)(wParam), EKeyInputState::RELEASED);
		break;
	}
	case WM_CHAR:
	{
		const TCHAR c = (TCHAR)wParam;
		bool isRepeat = lParam & 0xC0000000;
		Input::OnKeyChar(c, isRepeat);
		break;
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return 0;
}

void Window::SetDimension(uint width, uint height, EWindowMode mode)
{
	bool modeChanged = (uint8)mode != WindowMode;
	bool resChanged = width != Width || height != Height;
	if (!modeChanged && !resChanged)
	{
		return;
	}
	WindowMode = (uint8)mode;
	Width = width;
	Height = height;
	AspectRatio = (float)width / height;
	bool windowMode = mode != EWindowMode::FULLSCREEN;
	int scrWidth = GetSystemMetrics(SM_CXSCREEN);
	int scrHeight = GetSystemMetrics(SM_CYSCREEN);
	bool max = scrWidth == width;
	if (max)
	{
		if(EWindowMode::WINDOW == mode)
			width = GetSystemMetrics(SM_CXMAXIMIZED);
	}

	if (windowMode)
	{
		DWORD dwStyle = mode == EWindowMode::WINDOW ? WS_OVERLAPPED | WS_SYSMENU : WS_POPUP;
		int x = (scrWidth - width) / 2;
		int y = (scrHeight - height) / 2;
		RECT rect = {
			x,
			y,
			x + width,
			y + height };
		if(!max)
			AdjustWindowRect(&rect, dwStyle, mode == EWindowMode::WINDOW);
		if (modeChanged || max)
		{
			DestroyWindow(WindowHandle);
			dwStyle |= (max ? WS_MAXIMIZE : 0);
			WindowHandle = CreateWindow(Title.c_str(), Title.c_str(), dwStyle,
				x, y, rect.right - rect.left, rect.bottom - rect.top,
				NULL, NULL, GetModuleHandle(0), 0);
		}
		SetWindowPos(WindowHandle, HWND_TOP,
			x,
			y,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_SHOWWINDOW | SWP_FRAMECHANGED | (max ? SWP_NOSIZE | SWP_NOMOVE : 0));
	}
	else
	{

	}
}

HWND Window::GetConsoleHandle() const
{
#if defined(DEBUG) || defined(_DEBUG)
	return ConsoleHandle;
#else
	return NULL;
#endif
}
