#include "pch.h"
#include "Engine.h"
#include "Timer.h"
#include "../Window/Window.h"
#include "../Renderer/SceneRenderer.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "../UI/Console.h"

#define GFX Graphics::GetInstance()
#define WND Window::GetInstance()
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

	uint outputIdx = 0;
	GfxInstance->Resize(GfxInstance->FindBestOutput(outputIdx));
	Window::GetInstance()->OutputID = (uint8)outputIdx;
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
	InitConsoleCommands();
	return true;
}

bool Engine::InitConsoleCommands()
{
	CreateConsoleCommand(resize)([](std::vector<std::string>& params){
		if (params.size() <= 3)
		{
			RECT rect = GFX->GetOutputRect(WND->OutputID);
			RECT newRect = {};

			uint w = atoi(params[1].c_str());
			uint h = atoi(params[2].c_str());
			if (w == 0 || h == 0)
			{
				newRect = rect;\
			}
			else
			{
				uint hw = w >> 1;
				uint hh = h >> 1;;
				uint centX = rect.left + ((rect.right - rect.left) >> 1);
				uint centY = rect.top + ((rect.bottom - rect.top) >> 1);
				newRect.left = centX - hw;
				newRect.right = centX + hw;
				newRect.top = centY - hh;
				newRect.bottom = centY + hh;
				if (!IsInsideRect(newRect, rect))
				{
					newRect = rect;
				}
			}

			Engine::GetInstance()->Resize(newRect);
		}
	});
	return true;
}

void Engine::Run()
{
	g_Timer.Tick();
	GfxInstance->Tick();
	Console::GetInstance().ExecCommands();
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

void Engine::Resize(RECT& rect)
{
	GFX->GetDeviceResources()->WaitForGPU();
	ImGui_ImplDX12_InvalidateDeviceObjects();
	GfxInstance->Resize(rect);
	ImGui_ImplDX12_CreateDeviceObjects();
	DXGI_SWAP_CHAIN_DESC desc;
	GFX->GetDeviceResources()->GetSwapChain()->GetDesc(&desc);
	Console::GetInstance().AddLog("Window Size: %u, %u", desc.BufferDesc.Width, desc.BufferDesc.Height);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::WndProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
	{

		GfxInstance->GetDeviceResources()->WaitForGPU();
		ImGui_ImplDX12_InvalidateDeviceObjects();
		RECT rect;
		GetClientRect(Window::GetInstance()->GetHandle(), &rect);
		uint w = rect.right - rect.left;
		uint h = rect.bottom - rect.top;
		GFX->OnResize(w, h, w==0 || h==0 );
		ImGui_ImplDX12_CreateDeviceObjects();
	}
		break;
	case WM_INPUT:
	{

		break;
	}
	case WM_EXITSIZEMOVE:
	{
		RECT rect = {};
		POINT cur;
		GetPhysicalCursorPos(&cur);
		rect.left = cur.x;
		rect.top = cur.y;
		uint outputIdx = 0;
		rect = GfxInstance->FindBestOutput(rect, outputIdx);
		RECT windowRect = {};
		GetWindowRect(Window::GetInstance()->GetHandle(), &windowRect);
		if (!IsInsideRect(windowRect, rect))
		{
			Window::GetInstance()->OutputID = (uint8)outputIdx;
			Resize(rect);
		}
	}
		break;
	}
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
