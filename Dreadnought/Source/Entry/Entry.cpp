#include <stdafx.h>
#include <Windows/Window.h>
#include <Engine.h>
#include <iostream>
#include <stdlib.h>
#include <Vector.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdline, int nCmdShow){
#if defined(DEBUG) || defined(_DEBUG)
	//_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF); 
	//_CrtSetBreakAlloc(692);
#endif
	gEngine = std::make_unique<Engine>();
	gEngine->OnInit();

	const float Step = 1000000;
	const float StepSize = TWO_PI / Step;
	float angle0 = 0, angle1 = 0;
	_Vector2 v1(0), v2(0);
	_Vector4 v(1152.24f, 1024.166f, 1024.5661f);
	auto s = v.ToString();
	Timer timer;
	timer.Reset();
	for (float i = 0; i < TWO_PI; i += StepSize)
	{
		angle0 += v1.HeadingAngle();
		v1.y += i;
	}
	timer.Tick();
	auto a = timer.Elapsed();
	
	Timer timer1;
	timer1.Reset();
	for (float i = 0; i < TWO_PI; i += StepSize)
	{
		angle1 += GMath::Atan2(v2.y, v2.x);
		v2.y += i;
	}
	timer1.Tick();
	auto b = timer1.Elapsed();

	gEngine->Run();

	gEngine->OnDestroy();

	gEngine = nullptr;
#if defined(DEBUG) || defined(_DEBUG)
	//_CrtDumpMemoryLeaks();
#endif
	return 0;
}