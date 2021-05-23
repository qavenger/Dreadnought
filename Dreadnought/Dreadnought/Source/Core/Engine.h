#pragma once
class Engine
{
public:
	~Engine();
	static Engine* GetInstance();
	bool Init(class Window* Window);
	void Run();
private:
	Engine() {};
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
private:

	
};

