#include "pch.h"
#include "Engine.h"
#include "Timer.h"
#include "../Window/Window.h"
#include "../Renderer/SceneRenderer.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "../UI/Console.h"
Engine::~Engine()
{
	if (GetInstance())
	{
		delete GetInstance();
	}
	
}

Engine* Engine::GetInstance()
{
	static Engine* engine = nullptr;
	if (!engine)
	{
		engine = new Engine();
	}
	return engine;
}

bool Engine::Init()
{
	ShowConsole = false;

	Input::BindKeyInput(Input::EKeyCode::Tilde, EKeyInputState::RELEASED, []() {
		Engine::GetInstance()->GetShowConsole() = !Engine::GetInstance()->GetShowConsole();
	});


	Window& Wnd = *Window::GetInstance();
	Wnd.SetTitle(L"Engine");
	Wnd.SetWindowMode(EWindowMode::WINDOW);
	Wnd.Init_Wnd();
	Input::Start(Wnd.GetHandle());

	GfxInstance = SceneRenderer::GetInstance();

	if (!GfxInstance->Init())
	{
		return false;
	}
	GfxInstance->Resize(0);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(Wnd.GetHandle());
	ImGui_ImplDX12_Init(
		GfxInstance->GetDeviceResources()->GetDevice(),
		GfxInstance->GetDeviceResources()->GetBackBufferCount(),
		GfxInstance->GetDeviceResources()->GetBackBufferFormat(),
		GfxInstance->GetGUISRV(),
		GfxInstance->GetGUISRV()->GetCPUDescriptorHandleForHeapStart(),
		GfxInstance->GetGUISRV()->GetGPUDescriptorHandleForHeapStart()
		);
	return true;
}

void Engine::Run()
{
	g_Timer.Tick();
	GfxInstance->Tick();
}

void Engine::OnDestroy()
{
	Console::OnDestory();
	GfxInstance->GetDeviceResources()->WaitForGPU();
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	GfxInstance->OnDestroy();
	Window::GetInstance()->OnDestroy();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	switch (msg)
	{
	case WM_SIZE:

		ImGui_ImplDX12_InvalidateDeviceObjects();
		wprintf_s(L"%u x %u\n", LOWORD(lParam), HIWORD(lParam));
		//if (Engine::GetInstance()->GetRenderer()->IsInited())
		{
			RECT windowRect = {};
			GetWindowRect(Window::GetInstance()->GetHandle(), &windowRect);

			RECT clientRect = {};
			GetClientRect(Window::GetInstance()->GetHandle(), &clientRect);
			Engine::GetInstance()->GetRenderer()->OnResize(
				clientRect.right - clientRect.left,
				clientRect.bottom - clientRect.top,
				wParam == SIZE_MINIMIZED
			);
		}
		ImGui_ImplDX12_CreateDeviceObjects();
		break;
	}
	return 0;
}
