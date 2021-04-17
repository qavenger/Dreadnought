#pragma once

#if defined(DEBUG) || defined(_DEBUG)
	#define PrintDebugMessage(msg) wprintf(L"%s\n", msg)
	#define PrintDebugInt(msg) wprintf(L"%d\n", msg)
	#define PrintDebugFloat(msg) wprintf(L"%f\n", msg)
#else
	#define PrintDebugMessage(msg) 
	#define PrintDebugInt(msg)
	#define PrintDebugFloat(msg)
#endif
