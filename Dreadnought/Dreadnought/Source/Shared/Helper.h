#pragma once

#if defined(DEBUG) || defined(_DEBUG)
	#define PrintDebugMessage(msg, ...) wprintf(msg, __VA_ARGS__);wprintf(L"\n")
	#define PrintDebugInt(msg) wprintf(L"%d\n", msg)
	#define PrintDebugFloat(msg) wprintf(L"%f\n", msg)
#else
	#define PrintDebugMessage(msg) 
	#define PrintDebugInt(msg)
	#define PrintDebugFloat(msg)
#endif
