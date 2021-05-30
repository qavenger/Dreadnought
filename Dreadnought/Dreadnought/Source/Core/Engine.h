#pragma once
#include "Graphics/Graphics.h"
class Engine
{
public:
	~Engine();
	static Engine* GetInstance();
	bool Init();
	void Run();
	void OnDestroy();
public:
	Graphics* GetRenderer() { return GfxInstance; }
private:
	Engine() {};
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
private:
	Graphics* GfxInstance = nullptr;
};

