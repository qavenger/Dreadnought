#pragma once
#include <Windows/Window.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "UserInterface/Console.h"
class Engine : public INoncopyable
{
public:
	Engine();
	virtual ~Engine() {}
	void OnInit();
	void OnDestroy();
	void Run();
	void RegisterSubsystem(const tchar* name, ISubsystem* system);
	static ISubsystem* GetSubsystem(const tchar* systemName);
	LRESULT WindowProc(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam);
	bool& GetShowConsole() { return bShowConsole; }
private:
	void ToggleShowConsole() { bShowConsole = !bShowConsole; }
private:
	void PreTick(float dt);
	void Tick(float dt);
	void PostTick(float dt);
private:
	std::vector<char> RawInputBuffer;
	std::vector<ISubsystem*> Systems;
	std::map<std::string, ISubsystem*> SystemLookup;
	static Timer GlobalTimer;
	bool bShowConsole = false;
	std::unique_ptr<Console>	pConsole;
public:
	static std::unique_ptr<Engine> s_Engine;
	static float GetDeltaTime() { return GlobalTimer.DeltaTime(); }
	static double GetGameTime() { return GlobalTimer.Elapsed(); }
	static double GetTotalTime() { return GlobalTimer.UnscaledElapsed(); }
};
