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

	gEngine->Run();

	gEngine->OnDestroy();

	gEngine = nullptr;
#if defined(DEBUG) || defined(_DEBUG)
	//_CrtDumpMemoryLeaks();
#endif
	return 0;
}