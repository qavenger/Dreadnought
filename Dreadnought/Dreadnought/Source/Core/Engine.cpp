#include "pch.h"
#include "Engine.h"
#include "../Window/Window.h"
#include "../Graphics/Graphics.h"

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

bool Engine::Init(Window* Window)
{
	if (!Graphics::GetInstance()->Init(Window))
	{
		return false;
	}
	Graphics::GetInstance()->PostInit();
	return true;
}

void Engine::Run()
{
	
}
