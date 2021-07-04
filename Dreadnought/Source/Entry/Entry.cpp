#include <stdafx.h>
#include <Windows/Window.h>
#include <Engine.h>
#include <iostream>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdline, int nCmdShow){
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF); 
	//_CrtSetBreakAlloc(154);
#endif
	gEngine = std::make_unique<Engine>();

	gEngine->OnInit();


	//WindowPool* wndPool = (WindowPool*)ISubsystem::GetSubsystem("WindowPool");
	//if (wndPool)
	//{
	//	Window* wnd1 = wndPool->New("0");
	//	Window* wnd2 = wndPool->New("1", wnd1);
	//	Window* wnd3 = wndPool->New("2", wnd2);
	//	Window* wnd4 = wndPool->New("3", wnd3);
	//}
	gEngine->Run();

	gEngine->OnDestroy();

	gEngine = nullptr;
#if defined(DEBUG) || defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}