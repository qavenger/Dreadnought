#pragma once
DEFINE_DELEGATE_TWO_PARAM(WindowResizeEvent, uint, uint);
class Window
{
public:
	bool Init_Wnd();
	void SetTitle(const wchar_t* title) { Title = title; }
	static LRESULT CALLBACK WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam);
	bool SetDimension(uint width, uint height, EWindowMode mode = EWindowMode::WINDOW);
	void GetDimension(uint& width, uint& height)const { width = Width; height = Height; }
	EWindowMode GetWindowMode()const { return (EWindowMode)WindowMode; }
	void SetWindowMode(EWindowMode windowMode) { WindowMode = (uint8)windowMode; }
	float GetAspectRatio()const { return AspectRatio; }
	HWND GetHandle()const { return WindowHandle; }
	HWND GetConsoleHandle()const;
private:
	HWND WindowHandle;
#if defined(DEBUG) || defined(_DEBUG)
	HWND ConsoleHandle;
#endif
	std::wstring Title;
	float AspectRatio;
	uint16 Width;
	uint16 Height;
	uint8 WindowMode : 2;
public:
	WindowResizeEvent OnWindowResized;
};

