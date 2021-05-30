#include "pch.h"
#include "Engine.h"
#include "Timer.h"
#include "../Window/Window.h"
#include "../Renderer/SceneRenderer.h"

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
	return true;
}

void Engine::Run()
{
	g_Timer.Tick();
	GfxInstance->Tick();
}

void Engine::OnDestroy()
{
	GfxInstance->OnDestroy();
	Window::GetInstance()->OnDestroy();
}
