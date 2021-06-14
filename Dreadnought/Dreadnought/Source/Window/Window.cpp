#include "pch.h"
#include "Window.h"
#include "Core/Engine.h"
#include "resource.h"

static Window* gWindow = nullptr;

Window* Window::GetInstance()
{
	if (!gWindow)
		gWindow = new Window();
	return gWindow;
}
void Window::OnDestroy()
{
	if (gWindow)
	{
		delete gWindow;
		gWindow = nullptr;
	}
}

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

	bool windowed = GetWindowMode() == EWindowMode::WINDOW;
	/*uint16 width, height;
	GetDimension(width, height);
	AspectRatio = (float)width / height;
	int x = fullScreen ? 0 : CW_USEDEFAULT;
	int y = fullScreen ? 0 : CW_USEDEFAULT;*/
	UINT dwStyle = windowed ?   WS_OVERLAPPED | WS_MAXIMIZEBOX | WS_MINIMIZEBOX : WS_POPUP;
	WindowHandle = CreateWindow(WindowClass::GetName(), Title.c_str(), dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, WindowClass::GetInstanceHandle(), Engine::GetInstance());
	
	ThrowLastError(WindowHandle != nullptr);

	RAWINPUTDEVICE rid = {};
	rid.usUsage = 0x02;
	rid.usUsagePage = 0x01;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		return false;
	}
	return true;
}

LRESULT Window::WndProcSetup(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Engine* const pEngine = static_cast<Engine*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pEngine));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::WndProc));
		return pEngine->WndProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}
	auto* const pEngine = reinterpret_cast<Engine*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pEngine->WndProc(hWnd, msg, wParam, lParam);
}

bool Window::SetDimension(RECT rect, EWindowMode mode)
{
	float dpi = GetDpiForWindow(GetHandle()) / 96.0f;
	uint width = (uint)((rect.right - rect.left)*dpi);
	uint height = (uint)((rect.bottom - rect.top)*dpi);
	bool modeChanged = (uint8)mode != WindowMode;
	bool resChanged = width != Width || height != Height;
	if (!modeChanged && !resChanged)
	{
		return false;
	}
	WindowMode = (uint8)mode;
	Width = width;
	Height = height;
	AspectRatio = (float)width / height;
	bool windowMode = mode != EWindowMode::FULLSCREEN;

	if (windowMode)
	{
		DWORD dwStyle =/* mode == EWindowMode::WINDOW ? */WS_OVERLAPPED | WS_MINIMIZEBOX;
		AdjustWindowRect(&rect, dwStyle, false);
		SetWindowPos(WindowHandle, HWND_TOPMOST,
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			SWP_SHOWWINDOW | SWP_FRAMECHANGED | SWP_NOZORDER);
	}

	return true;
}

HWND Window::GetConsoleHandle() const
{
#if defined(DEBUG) || defined(_DEBUG)
	return ConsoleHandle;
#else
	return NULL;
#endif
}

Window::~Window()
{
	DestroyWindow(WindowHandle);
}

Window::WindowClass Window::WindowClass::WndClass;

HINSTANCE Window::WindowClass::GetInstanceHandle() noexcept
{
	return WndClass.hInstance;
}

Window::WindowClass::WindowClass() noexcept
	:hInstance(GetModuleHandle(nullptr))
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS | CS_OWNDC;
	wc.hInstance = GetInstanceHandle();
	wc.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ENGINEICON), IMAGE_ICON, 32, 32, 0));
	wc.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ENGINEICON), IMAGE_ICON, 16, 16, 0));
	//wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.lpszMenuName = NULL;
	wc.lpszClassName = GetName();
	wc.lpfnWndProc = WndProcSetup;

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(WndClassName, hInstance);
}
