#pragma once
class Window : public IObject
{
	using Super = IObject;
	friend class Engine;
	friend class WindowPool;
public:
	Window(const tchar* title, int16 parent = 0);
	//Window& operator=(const Window& other);
	Window* GetParent();
	HWND GetParentWindowHandle();
	HWND GetWindowHandle();
	uint32 GetWidth()const { return WndRect.right - WndRect.left; }
	uint32 GetHeight()const { return WndRect.bottom - WndRect.top; }
	RECT GetRect()const { return WndRect; }
	const RECT& GetRect() { return WndRect; }
	void Resize(uint32 w, uint32 h);
	void Destroy();
	virtual void OnDestroy()override;
private:
	Window();
	static class WindowPool* SubSystem;
	HWND hWnd;
	RECT WndRect;
	int16 Parent;
	std::set<int16> Children;
	std::string Title;
};

class WindowPool : public ISubsystem
{
	friend class Engine;
	friend class Window;
public:
	virtual ~WindowPool();

	Window* New(const tchar* name, Window* parent = nullptr);
	std::map<HWND,int16>::iterator DestroyWindow(HWND wndToDestroy);
	const tchar* GetWndClassName()const { return WndClassName; }
	const HINSTANCE GetWndInstanceHandle()const { return hInstance; }
	const Window* GetWindowFromHandle(HWND hWnd)const;
	const Window* GetWindowFromIndex(int16 index)const;
	Window* GetWindowFromHandle(HWND hWnd);
	int16 GetWindowIndex(HWND hWnd)const;
	Window* GetWindowFromIndex(int16 index);
	virtual void OnInit() override;
	virtual void OnDestroy() override;
	void PendingKill(int16 index);
	virtual void OnPostTick(float dt) override;
	static HWND GetMainWindow();
private:
	HINSTANCE hInstance = nullptr; 
	static constexpr int16 MAX_NUM_WINDOWS = 255;
	static constexpr const tchar* WndClassName = "DreadnoughtEngine";
	std::vector<Window> Windows;
	std::vector<int16> EmptySlots;
	std::map<HWND, int16> WindowLookup;
	std::vector<int16> PendingKillList;
	static WindowPool* s_WindowPool;
};