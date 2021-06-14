#pragma once

class Window : public INoncopyable
{
private:
	class WindowClass : public INoncopyable
	{
	public:
		static const wchar_t* GetName() noexcept { return WndClassName; }
		static HINSTANCE GetInstanceHandle()noexcept;
	private:
		WindowClass()noexcept;
		~WindowClass();
		static constexpr const wchar_t* WndClassName = L"Dreadnought Engine";
		static WindowClass WndClass;
		HINSTANCE hInstance;
	};
	friend class Engine;
public:
	static Window* GetInstance();
	void OnDestroy();
	bool Init_Wnd();
	void SetTitle(const wchar_t* title) { Title = title; }
	static LRESULT CALLBACK WndProcSetup(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam);
	bool SetDimension(RECT rect, EWindowMode mode = EWindowMode::WINDOW);
	void GetDimension(uint& width, uint& height)const { width = Width; height = Height; }
	EWindowMode GetWindowMode()const { return (EWindowMode)WindowMode; }
	void SetWindowMode(EWindowMode windowMode) { WindowMode = (uint8)windowMode; }
	float GetAspectRatio()const { return AspectRatio; }
	HWND GetHandle()const { return WindowHandle; }
	HWND GetConsoleHandle()const;
public:
	~Window();
private:
	HWND WindowHandle;
	HINSTANCE HInstance;
#if defined(DEBUG) || defined(_DEBUG)
	HWND ConsoleHandle;
#endif
	std::wstring Title;
	float AspectRatio;
	uint16 Width;
	uint16 Height;
	uint8 WindowMode : 2;
	uint8 OutputID : 4;
};


