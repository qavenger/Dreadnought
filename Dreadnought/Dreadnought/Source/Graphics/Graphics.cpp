#include "pch.h"
#include <Graphics/Graphics.h>
#include <Window/Window.h>

static Graphics* Gfx = nullptr;
#define THROW_IF_FAILED(rs, msg) if(!SUCCEEDED(rs)){PrintDebugMessage(msg); return rs;}
Graphics::~Graphics()
{
	if (Gfx)
	{
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
	Input::BindKeyInput(Input::EKeyCode::Space, EKeyInputState::PRESSED, Graphics::GetInstance()->ReportLiveObject);
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
	Resize(GetNumResolution() - 5);
	assert(SUCCEEDED(CreateDeviceAndFence()));
	assert(SUCCEEDED(CreateCommandQueue()));
	PrintDebugMessage(L"D3D12CommandQueue is created");

	return true;
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
	pWindow->SetDimension(res.x, res.y,resizeMode);
}

HRESULT Graphics::LogAdapter()
{
	uint numAdapters = 0;
	IDXGIAdapter1* pAdapter = nullptr;
	IDXGIOutput* pOutput = nullptr;
	DXGI_ADAPTER_DESC adapterDesc = {};
	DXGI_OUTPUT_DESC outputDesc = {};
	HRESULT rs;

	while (pFactory->EnumAdapters1(numAdapters, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		rs = pAdapter->GetDesc(&adapterDesc);
		assert(SUCCEEDED(rs));
		if (adapterDesc.DedicatedVideoMemory > 0)
		{
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
	assert(pAdapters.size() > 0);
	return rs;
}

HRESULT Graphics::CreateDeviceAndFence()
{
	HRESULT rs;
	rs = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(pDevice.GetAddressOf()));
	THROW_IF_FAILED(rs, L"Failed to create D3D12 device");
	
	PrintDebugMessage(L"D3D12 device is created");

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

HRESULT Graphics::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	return pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&pRenderCmdQueue));
}

HRESULT Graphics::CreateViewDepandentResource()
{
	HRESULT rs;
	DXGI_SWAP_CHAIN_DESC1 desc = {};
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC d = {};
	pWindow->GetDimension(desc.Width, desc.Height);
	desc.BufferCount = BackBufferCount;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	
	rs = pFactory->CreateSwapChainForHwnd(pRenderCmdQueue.Get(), pWindow->GetHandle(), &desc, nullptr, nullptr, &pSwapChainInter);
	THROW_IF_FAILED(rs, L"Failed to create swap chain");
	rs = pSwapChainInter.As(&pSwapChain);
	THROW_IF_FAILED(rs, L"Failed to query swap chain");
	return rs;

}
