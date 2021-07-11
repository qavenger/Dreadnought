#include <stdafx.h>
#include "Engine.h"
#include "Windows/Window.h"
#include "Input.h"
#include "Engine/Rendering/DeferredRenderer.h"

#define IMPLEMENT_SUBSYSTEM(s) s* s##Var = new s(); RegisterSubsystem(#s, s##Var)

std::unique_ptr<Engine> Engine::s_Engine = nullptr;
Timer Engine::GlobalTimer;
Engine::Engine()
{
	IMPLEMENT_SUBSYSTEM(Input);
	IMPLEMENT_SUBSYSTEM(WindowPool);
//	IMPLEMENT_SUBSYSTEM(RHIDevice);
	IMPLEMENT_SUBSYSTEM(DeferredRenderer);
	RawInputBuffer.clear();
}

void Engine::OnInit()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	for (auto* i : Systems)
	{
		try
		{
			i->OnInit();
		}
		catch (HrException e)
		{
			MessageBox(nullptr, e.GetExceptionMessage(), e.GetType(), 0);
		}
		catch (MsgException e)
		{
			MessageBox(nullptr, e.GetExceptionMessage(), e.GetType(), 0);
		}
	}

	Input::BindKeyInputEvent(EKeyCode::Escape, EKeyInputState::RELEASED, []() {PostQuitMessage(0); });
	Input::BindKeyInputEvent(EKeyCode::Tilde, EKeyInputState::RELEASED, this, &Engine::ToggleShowConsole );
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	pConsole = std::make_unique<Console>();
	GlobalTimer.Reset();
}

void Engine::PreTick(float dt)
{
	gDevice->BeginFrame();

	for (auto* i : Systems)
	{  
		i->OnPreTick(dt);
	}
}

void Engine::PostTick(float dt)
{
	for (auto* i : Systems)
	{
		i->OnPostTick(dt);
	}

	gDevice->EndFrame();
}

void Engine::Tick(float dt)
{
	for (auto* i : Systems)
	{
		i->OnTick(dt);
	}
}

void Engine::OnDestroy()
{
	for (auto* i : Systems)
	{
		i->OnDestroy();
	}
	for (int i = (int)Systems.size() - 1; i >= 0; --i)
	{
		if (Systems[i])
		{
			delete Systems[i];
			Systems[i] = nullptr;
		}
	}
	SystemLookup.clear();
	Systems.clear();
	pConsole.reset();
	ImGui::DestroyContext();
}

void Engine::Run()
{
#if PLATFORM_WINDOWS
	MSG msg = {  };
	while (msg.message != WM_QUIT)
	{
		GlobalTimer.Tick();
		try {
			gEngine->PreTick(GlobalTimer.DeltaTime());
			while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			
			gEngine->Tick(GlobalTimer.DeltaTime());

			gEngine->PostTick(GlobalTimer.DeltaTime());
			
			
		}
		catch (HrException e)
		{
			if (e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET)
			{
				
			}
			else
				MessageBox(0, e.GetExceptionMessage(), e.GetType(), 0);
		}
		catch (MsgException e)
		{
			MessageBox(0, e.GetExceptionMessage(), e.GetType(), 0);
		}
		catch (...)
		{

		}
	}
#endif
}


void Engine::RegisterSubsystem(const tchar* name, ISubsystem* system)
{
	Systems.push_back(system);
	SystemLookup[name] = system;
}

ISubsystem* Engine::GetSubsystem(const tchar* systemName)
{
	auto itr = gEngine->SystemLookup.find(systemName);
	if (itr != gEngine->SystemLookup.end()) return itr->second;
	return nullptr;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::WindowProc(HWND hWnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)(wParam);
		if (hDrop)
		{
			uint32 numFile = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
			for (uint32 i = 0; i < numFile; ++i)
			{
				uint32 numChar = DragQueryFile(hDrop, i, nullptr, 0);
				if (numChar > 0)
				{
					std::string s;
					s.resize(numChar + 1u);
					numChar = DragQueryFile(hDrop, i, s.data(), numChar + 1u);
					MessageBox(nullptr, s.c_str(), "File", 0);
				}
			}
			DragFinish(hDrop);
		}
	}
		break;
	case WM_MOUSEWHEEL:
	{
		auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (delta != 0)
		{
			Input::OnMouseWheel(delta > 0 ? 1.0f : -1.0f);
		}
	}
		break;
	case WM_MOUSEMOVE:
		Input::OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		Input::OnMouseEvent(EMouseInputState::DOWN, 0);
		break;
	case WM_MBUTTONDOWN:
		Input::OnMouseEvent(EMouseInputState::DOWN, 1);
		break;
	case WM_RBUTTONDOWN:
		Input::OnMouseEvent(EMouseInputState::DOWN, 2);
		break;
	case WM_LBUTTONUP:
		Input::OnMouseEvent(EMouseInputState::UP, 0);
		break;
	case WM_MBUTTONUP:
		Input::OnMouseEvent(EMouseInputState::UP, 1);
		break;
	case WM_RBUTTONUP:
		Input::OnMouseEvent(EMouseInputState::UP, 2);
		break;
	case WM_LBUTTONDBLCLK:
		Input::OnMouseEvent(EMouseInputState::DBLCLK, 0);
		break;
	case WM_MBUTTONDBLCLK:
		Input::OnMouseEvent(EMouseInputState::DBLCLK, 1);
		break;
	case WM_RBUTTONDBLCLK:
		Input::OnMouseEvent(EMouseInputState::DBLCLK, 2);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if((lParam & 0xC0000000) == 0 ) // check if no auto repeat
			Input::OnKeyInput(EKeyInputState::PRESSED, (EKeyCode)wParam);
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		Input::OnKeyInput(EKeyInputState::RELEASED,(EKeyCode)wParam);
		break;
	case WM_CHAR:
		if(wParam != VK_ESCAPE)
			Input::OnChar((tchar)wParam, lParam & 0xC0000000);
		break;
	case WM_INPUT:
	{
		UINT size = 0;
		HRAWINPUT hInput = reinterpret_cast<HRAWINPUT>(lParam);
		if (GetRawInputData(
			hInput,
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1)break;
		
		if (size != RawInputBuffer.size())
		{
			RawInputBuffer.resize(size);
		}
		if (GetRawInputData(
			hInput
			, RID_INPUT
			, RawInputBuffer.data()
			, &size
			, sizeof(RAWINPUTHEADER)) != size)break;
		
		auto& ri = reinterpret_cast<const RAWINPUT&>(*RawInputBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE && ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0)
		{
			Input::OnMouseRawMove(ri.data.mouse.lLastX != 0, ri.data.mouse.lLastY);
		}
	}
		break;
	}

	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
