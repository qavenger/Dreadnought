#pragma once
// Platform related should be defined here
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
typedef TCHAR tchar;
#endif

typedef unsigned char byte;
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

#ifdef PLATFORM_WINDOWS
#include <DirectXMath.h>
//typedef DirectX::XMFLOAT2 float2;
//typedef DirectX::XMFLOAT3 float3;
//typedef DirectX::XMFLOAT4 float4;
typedef DirectX::XMINT2 int2;
typedef DirectX::XMINT3 int3;
typedef DirectX::XMINT4 int4;
typedef DirectX::XMUINT2 uint2;
typedef DirectX::XMUINT3 uint3;
typedef DirectX::XMUINT4 uint4;
#endif

#define DEFINE_DELEGATE_NO_PARAM(Name) \
typedef TMulticastDelegate<> Name;

#define DEFINE_DELEGATE_ONE_PARAM(Name, ParamType0) \
typedef TMulticastDelegate<ParamType0> Name;

#define DEFINE_DELEGATE_TWO_PARAM(Name, ParamType0, ParamType1) \
typedef TMulticastDelegate<ParamType0, ParamType1> Name;

#define DEFINE_DELEGATE_THREE_PARAM(Name, ParamType0, ParamType1, ParamType2) \
typedef TMulticastDelegate<ParamType0, ParamType1, ParamType2> Name;


#define NewObject(T, Name) gEngine->NewObject<T>(Name)

#define gEngine Engine::s_Engine
#define gDevice RHIDeviceCreator::Device

#if defined(_DEBUG) || defined(DEBUG)
#define NOENDBG noexcept(!_DEBUG)
#else
#define NOENDBG noexcept
#endif

// DX12 RHI
#define USE_DX12_RHI 1

#define MAX_BACK_BUFFER_COUNT 3
#define MAX_RENDER_TARGET 8

#if USE_DX12_RHI
#define SCOPE_EVENT(EventName) \
        struct EventName##_ScopeEvent\
	    {\
		    EventName##_ScopeEvent()\
            {\
                gDevice->BeginScopeEvent(#EventName);\
            }\
		    ~EventName##_ScopeEvent()\
           {\
                gDevice->EndScopeEvent();\
           }\
        } EventName##Local;
        
#define SCOPE_EVENT_STR_FORMAT(UniqueName, ...) \
        char EventName[100] = {0}; \
        sprintf(EventName, __VA_ARGS__); \
        struct UniqueName##_ScopeEventStr\
	    {\
		    UniqueName##_ScopeEventStr(const char* In)\
            {\
                gDevice->BeginScopeEvent(In);\
            }\
		    ~UniqueName##_ScopeEventStr()\
           {\
                gDevice->EndScopeEvent();\
           }\
        } UniqueName##StrLocal(EventName);
#else
#define SCOPE_EVENT(EventName)
#define SCOPE_EVENT_STR(EventName, Msg)
#endif