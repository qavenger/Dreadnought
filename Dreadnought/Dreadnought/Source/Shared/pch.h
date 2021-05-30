#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <thread>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#include <d3dx12.h>
#include <define.h>
#include <EngineTypes.h>
#include "Input/Input.h"
#include "Helper.h"
#include <shellapi.h>
#include <memory>
#include <assert.h>
#include <atlbase.h>
#include "Templates/TemplateClasses.h"

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

using Microsoft::WRL::ComPtr;