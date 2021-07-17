#pragma once

#include <sdkddkver.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOSYSCOMMANDS
#define NOMINMAX
#define NOIMAGE
#define _CRT_SECURE_NO_WARNINGS 1

#include <Define.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <StringUtil.h>
#include <unordered_map>
#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#include <../RHI/D3D12/d3dx12.h>
#include <EngineType.h>
//#include <Input.h>
#include "Timer.h"
#include "Error.h"
#include <shellapi.h>
#include <memory>
#include <assert.h>
#include <atlbase.h>
#include <atlcore.h>
#include <Engine/Abstract/InterfaceBase.h>
#include <Engine/Abstract/TemplatedBase.h>
#include <Engine/Abstract/Object.h>
#include <EngineMath.h>
#include <Engine.h>
#include <RHI/CommonEnums.h>
#include "RHI/RHIDeviceCreator.h"
#include "RHI/RHIDevice.h"
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3dcompiler.lib")

#ifdef PLATFORM_WINDOWS
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifdef PLATFORM_WINDOWS
#include <pix.h>
#endif

#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#endif

using Microsoft::WRL::ComPtr;
