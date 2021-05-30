#include "pch.h"
#include "D3D12DeviceResource.h"
#include "Window/Window.h"

namespace
{
	inline DXGI_FORMAT NoSRGB(DXGI_FORMAT InFormat)
	{
		switch (InFormat)
		{
		case DXGI_FORMAT_BC1_UNORM_SRGB:		return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_UNORM_SRGB:		return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_UNORM_SRGB:		return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_BC7_UNORM_SRGB:		return DXGI_FORMAT_BC7_UNORM;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:	return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:	return DXGI_FORMAT_B8G8R8X8_UNORM;
		default:								return InFormat;
		}
	}
}


D3D12DeviceResource::D3D12DeviceResource(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilBufferFormat, UINT numBackBuffer, D3D_FEATURE_LEVEL minFeatureLevel, UINT flag, UINT adapterIDOverride)
	:
	FenceValue{},
	AdapterIDOverride(adapterIDOverride),
	AdapterID(UINT_MAX),
	BackBufferIndex(0),

	BackBufferFormat(backBufferFormat),
	DepthStencilBufferFormat(depthStencilBufferFormat),
	NumBackBuffer(numBackBuffer),
	MinFeatureLevel(minFeatureLevel)
{
	if (numBackBuffer > MAX_BACK_BUFFER_COUNT)
	{
		throw std::out_of_range("Too many back buffers are assigned");
	}

	if (minFeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		throw std::out_of_range("Min feature level to low");
	}
}

bool D3D12DeviceResource::OnResize(int w, int h, bool minimized)
{
	IsWindowVisible = !minimized;

	if (minimized || w == 0 || h == 0)
	{
		return false;
	}

	RECT newRect = { 0, 0, max(1,w), max(1,h) };
	if (
		newRect.left == OutputSize.left &&
		newRect.right == OutputSize.right &&
		newRect.top == OutputSize.top &&
		newRect.bottom == OutputSize.bottom
		)
	{
		return false;
	}
	OutputSize = newRect;
	CreateWindowDependentResources();
	return true;
}

void D3D12DeviceResource::RegisterDeviceNotify(IDeviceNotify* Notifier)
{
	DeviceNotify = Notifier;

	__if_exists(DXGIDeclareAdapterRemovalSupport)
	{
		if (DeviceNotify)
		{
			if (FAILED(DXGIDeclareAdapterRemovalSupport()))
			{
				PrintDebugMessage(L"WARNING: application failed to declare adapter removal support.\n");
			}
		}
	}
}

void D3D12DeviceResource::InitDXGIAdapter()
{
	bool debugDXGI = false;

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
		else
		{
			OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
		}

		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
		{
			debugDXGI = true;

			ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&Factory4)));

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
	}
#endif
	if (Options)
	{
		BOOL allowTearing = FALSE;
		ComPtr<IDXGIFactory5> factory5;
		HRESULT hr = Factory4.As(&factory5);
		if (SUCCEEDED(hr))
		{
			hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
		}

		if (FAILED(hr) || !allowTearing)
		{
			PrintDebugMessage(L"WARNING: Variable refresh rate displays are not supported");
			//TODO: Tearing support
		}
	}

	if (!debugDXGI)
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&Factory4)));
	}

	InitAdapter(&Adapter);
	CreateDeviceResources();
}

void D3D12DeviceResource::CreateDeviceResources()
{
	ThrowIfFailed(D3D12CreateDevice(Adapter.Get(), MinFeatureLevel, IID_PPV_ARGS(&Device)));
#ifdef _DEBUG
	// Configure debug device (if active).
	ComPtr<ID3D12InfoQueue> d3dInfoQueue;
	if (SUCCEEDED(Device.As(&d3dInfoQueue)))
	{
#ifdef _DEBUG
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
		D3D12_MESSAGE_ID hide[] =
		{
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
		};
		D3D12_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif

	// Determine maximum supported feature level for this device
	static const D3D_FEATURE_LEVEL s_featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
	{
		_countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
	};

	HRESULT hr = Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
	if (SUCCEEDED(hr))
	{
		MaxFeatureLevel = featLevels.MaxSupportedFeatureLevel;
	}
	else
	{
		MaxFeatureLevel = MinFeatureLevel;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowIfFailed(Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&RenderCommandQueue)));
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = NumBackBuffer;
		ThrowIfFailed(Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&RTVDescriptorHeap)));

		rtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		if (DepthStencilBufferFormat != DXGI_FORMAT_UNKNOWN)
		{
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			desc.NumDescriptors = 1;
			ThrowIfFailed(Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&DSVDescriptorHeap)));
		}
	}

	for (uint i = 0; i < NumBackBuffer; ++i)
	{
		ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator[i])));
	}

	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator[0].Get(),nullptr, IID_PPV_ARGS(&CommandList)));
	ThrowIfFailed(CommandList->Close());
	ThrowIfFailed(Device->CreateFence(FenceValue[BackBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
	++FenceValue[BackBufferIndex];
	FenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
	if (!FenceEvent.IsValid())
	{
		ThrowIfFailed(E_FAIL, L"Create fence event failed\n");
	}
}

void D3D12DeviceResource::CreateWindowDependentResources()
{
	WaitForGPU();

	for (uint n = 0; n < NumBackBuffer; ++n)
	{
		RenderTargets[n].Reset();
		FenceValue[n] = FenceValue[BackBufferIndex];
	}

	uint backBufferWidth = max(OutputSize.right - OutputSize.left, 1);
	uint backBufferHeight = max(OutputSize.bottom - OutputSize.top, 1);
	DXGI_FORMAT backBufferFormat = NoSRGB(BackBufferFormat);

	if (SwapChain)
	{
		HRESULT hr = SwapChain->ResizeBuffers(
			NumBackBuffer,
			backBufferWidth,
			backBufferHeight,
			backBufferFormat,
			0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			PrintDebugMessage(L"Device Lost on ResizeBuffers: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? Device->GetDeviceRemovedReason() : hr);
			HandleDeviceLost();
			return;
		}
		else
		{
			ThrowIfFailed(hr);
		}
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = backBufferWidth;
		desc.Height = backBufferHeight;
		desc.Format = backBufferFormat;
		desc.BufferCount = NumBackBuffer;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
		fullscreenDesc.Windowed = TRUE;

		ComPtr<IDXGISwapChain1> swapChain;

		ThrowIfFailed(Factory4->CreateSwapChainForHwnd(RenderCommandQueue.Get(), Window::GetInstance()->GetHandle(), &desc, &fullscreenDesc, nullptr, &swapChain));
		ThrowIfFailed(swapChain.As(&SwapChain));
	}

	for (uint bufferIdx = 0; bufferIdx < NumBackBuffer; ++bufferIdx)
	{
		ThrowIfFailed(SwapChain->GetBuffer(bufferIdx, IID_PPV_ARGS(&RenderTargets[bufferIdx])));

#ifdef _DEBUG
		std::wstring name = L"Render Target ";
		name += bufferIdx;
		ThrowIfFailed(RenderTargets[bufferIdx]->SetName(name.c_str()));
#endif

		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = BackBufferFormat;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), bufferIdx, rtvDescriptorSize);
		Device->CreateRenderTargetView(RenderTargets[bufferIdx].Get(), &viewDesc, handle);
	}
	BackBufferIndex = SwapChain->GetCurrentBackBufferIndex();

	if (DXGI_FORMAT_UNKNOWN != DepthStencilBufferFormat)
	{
		CD3DX12_HEAP_PROPERTIES depthStencilHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DepthStencilBufferFormat, backBufferWidth, backBufferHeight, 1, 1);
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE depthClear = {};
		depthClear.DepthStencil.Depth = 1;
		depthClear.DepthStencil.Stencil = 0;
		depthClear.Format = DepthStencilBufferFormat;

		ThrowIfFailed(Device->CreateCommittedResource(
			&depthStencilHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClear, IID_PPV_ARGS(&DepthStencil)
		));

#ifdef _DEBUG
		ThrowIfFailed(DepthStencil->SetName(L"DepthStencil"));
#endif

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DepthStencilBufferFormat;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		Device->CreateDepthStencilView(DepthStencil.Get(), &dsv, DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	}

	ScreenViewport.TopLeftX = ScreenViewport.TopLeftY = 0;
	ScreenViewport.Width = (float)backBufferWidth;
	ScreenViewport.Height = (float)backBufferHeight;
	ScreenViewport.MinDepth = D3D12_MIN_DEPTH;
	ScreenViewport.MaxDepth = D3D12_MAX_DEPTH;

	ScissorRect = { 0, 0, (LONG)backBufferWidth, (LONG)backBufferHeight };
}

void D3D12DeviceResource::WaitForGPU() noexcept
{
	if (RenderCommandQueue && Fence && FenceEvent.IsValid())
	{
		uint64 fenceValue = FenceValue[BackBufferIndex];
		if (SUCCEEDED(RenderCommandQueue->Signal(Fence.Get(), fenceValue)))
		{
			if (SUCCEEDED(Fence->SetEventOnCompletion(fenceValue, FenceEvent.Get())))
			{
				WaitForSingleObjectEx(FenceEvent.Get(), INFINITE, FALSE);
				++FenceValue[BackBufferIndex];
			}
		}
	}
}

void D3D12DeviceResource::ExecuteCommandLists()
{
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* Lists[] = { CommandList.Get() };
	RenderCommandQueue->ExecuteCommandLists(ARRAYSIZE(Lists), Lists);
}

void D3D12DeviceResource::HandleDeviceLost()
{
	if (DeviceNotify)
	{
		DeviceNotify->OnDeviceLost();
	}

	for (uint n = 0; n < NumBackBuffer; ++n)
	{
		CommandAllocator[n].Reset();
		RenderTargets[n].Reset();
	}

	DepthStencil.Reset();
	RenderCommandQueue.Reset();
	CommandList.Reset();
	Fence.Reset();
	RTVDescriptorHeap.Reset();
	DSVDescriptorHeap.Reset();
	SwapChain.Reset();
	Device.Reset();
	Factory4.Reset();
	Adapter.Reset();

#ifdef _DEBUG
	{
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif

	InitDXGIAdapter();
	CreateDeviceResources();
	CreateWindowDependentResources();
	if (DeviceNotify)
	{
		DeviceNotify->OnDeviceRestored();
	}
}

void D3D12DeviceResource::BeginFrame(D3D12_RESOURCE_STATES beforeState)
{
	ThrowIfFailed(CommandAllocator[BackBufferIndex]->Reset());
	ThrowIfFailed(CommandList->Reset(CommandAllocator[BackBufferIndex].Get(), nullptr));
	if (D3D12_RESOURCE_STATE_RENDER_TARGET != beforeState)
	{
		D3D12_RESOURCE_BARRIER barrier =
			CD3DX12_RESOURCE_BARRIER::Transition(
				RenderTargets[BackBufferIndex].Get(),
				beforeState,
				D3D12_RESOURCE_STATE_RENDER_TARGET
			);
		CommandList->ResourceBarrier(1, &barrier);
	}
	// Do stuff here...
}

void D3D12DeviceResource::EndFrame(D3D12_RESOURCE_STATES beforeState)
{
	// Or here...

	if (beforeState != D3D12_RESOURCE_STATE_PRESENT)
	{
		D3D12_RESOURCE_BARRIER barrier =
			CD3DX12_RESOURCE_BARRIER::Transition(
				RenderTargets[BackBufferIndex].Get(),
				beforeState,
				D3D12_RESOURCE_STATE_PRESENT
			);
		CommandList->ResourceBarrier(1, &barrier);
	}

	ExecuteCommandLists();

	HRESULT hr = S_OK;

	hr = SwapChain->Present(1, 0);

	if (DXGI_ERROR_DEVICE_REMOVED == hr || DXGI_ERROR_DEVICE_RESET == hr)
	{
		PrintDebugMessage(L"Device Lost On Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED ? Device->GetDeviceRemovedReason() : hr));

		HandleDeviceLost();
	}
	else
	{
		ThrowIfFailed(hr);
		MoveToNextFrame();
	}
}

void D3D12DeviceResource::MoveToNextFrame()
{
	const uint64 currentFenceValue = FenceValue[BackBufferIndex];
	ThrowIfFailed(RenderCommandQueue->Signal(Fence.Get(), currentFenceValue));

	BackBufferIndex = SwapChain->GetCurrentBackBufferIndex();

	if (Fence->GetCompletedValue() < FenceValue[BackBufferIndex])
	{
		ThrowIfFailed(Fence->SetEventOnCompletion(FenceValue[BackBufferIndex], FenceEvent.Get()));
		WaitForSingleObjectEx(FenceEvent.Get(), INFINITE, FALSE);
	}

	FenceValue[BackBufferIndex] = currentFenceValue + 1;
}

void D3D12DeviceResource::InitAdapter(IDXGIAdapter1** ppAdapter)
{
	*ppAdapter = nullptr;
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<IDXGIFactory6> factory6;

	ThrowIfFailed(Factory4.As<IDXGIFactory6>(&factory6));

	for (uint adapterId = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(adapterId, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapterId)
	{
		if (AdapterIDOverride != UINT_MAX && adapterId != AdapterIDOverride)
		{
			continue;
		}
		DXGI_ADAPTER_DESC1 desc;
		ThrowIfFailed(adapter->GetDesc1(&desc));

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(
				adapter.Get(), 
				MinFeatureLevel, 
				_uuidof(ID3D12Device),
				nullptr)
		))
		{
			AdapterID = adapterId;
			AdapterDescription = desc.Description;

			ComPtr<IDXGIOutput> output;
			std::set<uint> uniqueResolutions;
			for (uint i = 0;  DXGI_ERROR_NOT_FOUND!= adapter->EnumOutputs(i, &output); ++i)
			{
				uint numModes = 0;
				ThrowIfFailed(output->GetDisplayModeList(BackBufferFormat, 0, &numModes, nullptr));
				std::vector<DXGI_MODE_DESC> modes;
				modes.resize(numModes);
				ThrowIfFailed(output->GetDisplayModeList(BackBufferFormat, 0, &numModes, &modes[0]));
				for (auto& mode : modes)
				{
					uniqueResolutions.insert((mode.Width) | (mode.Height << 16));
				}
			}

			for (auto r : uniqueResolutions)
			{
				SupportedResolutions.push_back(r);
			}
			SelectedResolution = (uint)SupportedResolutions.size() - 1;

			PrintDebugMessage(L"Video Card (%u): VRAM: %llu, Name: %ws\n", adapterId, desc.DedicatedVideoMemory, desc.Description);
			break;
		}
	}

	if (!adapter)
	{
		if (AdapterIDOverride != UINT_MAX)
		{
			throw std::exception("Unavailable adapter requested.");
		}
		else
		{
			throw std::exception("Unavailable adapter.");
		}
	}

	*ppAdapter = adapter.Detach();
}
