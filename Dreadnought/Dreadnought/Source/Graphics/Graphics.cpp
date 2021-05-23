#include "pch.h"
#include <Graphics/Graphics.h>
#include <Window/Window.h>

static Graphics* Gfx = nullptr;
#define THROW_IF_FAILED(rs, msg) if(!SUCCEEDED(rs)){PrintDebugMessage(msg); return rs;}
Graphics::~Graphics()
{
	if (Gfx)
	{
		for (auto* outputs : pOutputs)
		{
			outputs->Release();
			outputs = nullptr;
		}
		for (auto* adapter : pAdapters)
		{
			adapter->Release();
			adapter = nullptr;
		}
		pAdapters.clear();
		pOutputs.clear();
		delete Gfx;
		Gfx = nullptr;
	}
}

Graphics* Graphics::GetInstance()
{
	if (!Gfx)
	{
		Gfx = new Graphics();
	}
	return Gfx;
}

bool Graphics::Init(Window* Wnd)
{
	pWindow = Wnd;

	HRESULT rs;
	uint factoryflag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12Debug> pDebug;
	rs = D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.GetAddressOf()));
	assert(SUCCEEDED(rs));
	PrintDebugMessage(L"Debug layer is created");
	pDebug->EnableDebugLayer();
	factoryflag = DXGI_CREATE_FACTORY_DEBUG;
#endif
	rs = CreateDXGIFactory2(factoryflag,IID_PPV_ARGS(pFactory.GetAddressOf()));
	assert(SUCCEEDED(rs));
	PrintDebugMessage(L"DXGI Factory is created");
	PrintDebugMessage(L"================================= ");
	PrintDebugMessage(L"	Querying Adapters ");
	PrintDebugMessage(L"================================= \n");
	assert(SUCCEEDED(LogAdapter()));
	assert(SUCCEEDED(CreateDeviceAndFence()));
	assert(SUCCEEDED(CreateCommandObjects()));
	PrintDebugMessage(L"D3D12Command objects are created");
	return true;
}

void Graphics::PostInit()
{
	Input::BindKeyInput(Input::EKeyCode::Up, EKeyInputState::RELEASED, []() {
		if (Input::IsKeyDown(Input::EKeyCode::Shift)) {
			Graphics::GetInstance()->Resize(Graphics::GetInstance()->PrevResolution());
		}
	});


	pWindow->OnWindowResized.Bind(
		[](uint w, uint h) {
		Graphics::GetInstance()->ReleaseViewDepandentResource();
		Graphics::GetInstance()->CreateViewDepandentResource(w, h);
	});

	Resize(GetNumResolution() - 5);
}

void Graphics::ReportLiveObject()
{
#if defined(DEBUG) || defined(_DEBUG)
	ComPtr<ID3D12DebugDevice2> pDebug;
	HRESULT rs = GetInstance()->pDevice->QueryInterface(IID_PPV_ARGS(&pDebug));
	if (SUCCEEDED(rs))
	{
		pDebug->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	}
#endif
}

// Resolution : low 16 bit = width(x), height 16 bit = height(y)
uint2 Graphics::GetResolution(int index) const
{
	uint rs = uResolutions.back();
	if (index >= 0 && index < uResolutions.size())
	{
		rs = uResolutions[index];
	}
	return uint2(rs & 0xffff, (rs >> 16) & 0xffff);
}

void Graphics::Resize(int resolutionIndex, EWindowMode resizeMode)
{
	uint2 res = GetResolution(resolutionIndex);
	pWindow->SetDimension(res.x, res.y, resizeMode);
}

void Graphics::Resize(uint width, uint height, EWindowMode resizeMode)
{
	pWindow->SetDimension(width, height, resizeMode);
}

HRESULT Graphics::LogAdapter()
{
	uint numAdapters = 0;
	IDXGIAdapter1* pAdapter = nullptr;
	IDXGIOutput* pOutput = nullptr;
	DXGI_ADAPTER_DESC adapterDesc = {};
	DXGI_OUTPUT_DESC outputDesc = {};
	HRESULT rs = 0;
	uint curBest = 0;
	while (pFactory->EnumAdapters1(numAdapters, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		rs = pAdapter->GetDesc(&adapterDesc);
		assert(SUCCEEDED(rs));
		if (adapterDesc.DedicatedVideoMemory > 0)
		{
			if (adapterDesc.DedicatedVideoMemory > curBest)
			{
				curBest = (uint)adapterDesc.DedicatedVideoMemory;
				SelectedAdapter = numAdapters;
			}
			std::wstring s = L"Video Card: ";
			s += adapterDesc.Description;
			s += L"\nVideo Memory: ";
			SIZE_T vmSize = adapterDesc.DedicatedVideoMemory / (1024 * 1024);
			wchar_t wSize[64];
			_ui64tow_s(vmSize, wSize, 64, 10);
			s += wSize;
			s+= L" MB\n";
			PrintDebugMessage(s.c_str());
			pAdapters.push_back(pAdapter);
			uint numOutputs = 0;
			while (pAdapter->EnumOutputs(numOutputs, &pOutput) != DXGI_ERROR_NOT_FOUND)
			{
				uint numMode = 0;
				rs = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numMode, nullptr);
				assert(SUCCEEDED(rs));
				std::vector<DXGI_MODE_DESC> modes(numMode);
				rs = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numMode, modes.data());
				assert(SUCCEEDED(rs));

				for (uint modeIdx = 0; modeIdx < numMode; ++modeIdx)
				{
					uint res = modes[modeIdx].Width | (modes[modeIdx].Height << 16);
					if (uResolutions.empty() || uResolutions.back() != res)
					{
						uResolutions.push_back(res);
					}
				}
				uResolutions.shrink_to_fit();
				PrintDebugMessage(L"Resolutions:");
				for (uint res : uResolutions)
				{
					uint w, h;
					w = res & 0x0000ffff;
					h = res >> 16;
					wprintf_s(L"%u x %u\n", w, h);
				} 
				wprintf_s(L"\n");
				pOutputs.push_back(pOutput);
				pOutput->GetDesc(&outputDesc);
				++numOutputs;
			}
		}
		++numAdapters;
	}
	pAdapters.shrink_to_fit();
	pOutputs.shrink_to_fit();
	SelectedResolution = (uint)uResolutions.size() - 1;
	assert(pAdapters.size() > 0);
	return rs;
}

HRESULT Graphics::CreateDeviceAndFence()
{
	HRESULT rs;
	rs = D3D12CreateDevice(pAdapters[SelectedAdapter], D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(pDevice.GetAddressOf()));
	THROW_IF_FAILED(rs, L"Failed to create D3D12 device");

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 option;
	D3D12_FEATURE_DATA_D3D12_OPTIONS6 option6;
	pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, (void*)&option, sizeof(option));
	pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, (void*)&option6, sizeof(option6));

	PrintDebugMessage(L"D3D12 device is created");
	PrintDebugMessage(L"Raytracing Tier, %d", (int)option.RaytracingTier);
	PrintDebugMessage(L"VSR Tier, %d", (int)option6.VariableShadingRateTier);

	rs = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
	THROW_IF_FAILED(rs, L"Failed to create fence");

	PrintDebugMessage(L"D3D12 fence is created");

	for (UINT i = 0; i < (UINT)D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
	{
		DescriptorHandleIncrementSize[i] = pDevice->GetDescriptorHandleIncrementSize((D3D12_DESCRIPTOR_HEAP_TYPE)i);
		PrintDebugInt(DescriptorHandleIncrementSize[i]);
	}
	 
	return rs;
}

HRESULT Graphics::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	THROW_IF_FAILED(pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&pRenderCmdQueue)), L"Failed To Create Command Queue");
	THROW_IF_FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pDirectCmdListAlloc)), L"Failed To Create Command Allocator");
	THROW_IF_FAILED(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pDirectCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(&pCommandList)), L"Failed To Create Command List");
	pCommandList->Close();
	return S_OK;
}

HRESULT Graphics::CreateViewDepandentResource(uint Width, uint Height)
{
	HRESULT rs;
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC d = {};
	pWindow->GetDimension(Width, Height);
	desc.BufferCount = BackBufferCount;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> pSwapChainInter;
	rs = pFactory->CreateSwapChainForHwnd(pRenderCmdQueue.Get(), pWindow->GetHandle(), &desc, nullptr, nullptr, &pSwapChainInter);
	THROW_IF_FAILED(rs, L"Failed to create swap chain");
	rs = pSwapChainInter.As(&pSwapChain);
	THROW_IF_FAILED(rs, L"Failed to query swap chain");

	DXGI_SWAP_CHAIN_DESC descOut;
	pSwapChain->GetDesc(&descOut);
	PrintDebugMessage(L"Width: %d, Height: %d", descOut.BufferDesc.Width, descOut.BufferDesc.Height);

	return rs;
}

void Graphics::ReleaseViewDepandentResource()
{
	if (pSwapChain)
	{
		pSwapChain = nullptr;
	}
}

uint Graphics::PrevResolution()
{
	if (--SelectedResolution >= (uint)uResolutions.size())
	{
		SelectedResolution = (uint)uResolutions.size() - 1;
	}
	return SelectedResolution;
}
