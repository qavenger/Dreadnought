#pragma once
#include "Graphics/Graphics.h"
class Engine
{
public:
	~Engine();
	static Engine* GetInstance();
	bool Init();
	bool InitConsoleCommands();
	void Run();
	void OnDestroy();
	bool& GetShowConsole() { return ShowConsole; }
	void Resize(RECT& rect);
public:
	Graphics* GetRenderer() { return GfxInstance; }
	LRESULT WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam);
private:
	Engine() {};
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
private:
	Graphics* GfxInstance = nullptr;
	bool ShowConsole = false;
private:
	uint16			bWindowIsMovingSizing : 1;
};

