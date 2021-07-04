#include <stdafx.h>
#include "D3D12DeviceResource.h"

D3D12DeviceResource::D3D12DeviceResource(
	DXGI_FORMAT backBufferFormat,
	DXGI_FORMAT depthStencilBufferFormat,
	uint32 backBufferCount,
	D3D_FEATURE_LEVEL minFeatureLevel,
	uint32 flags, 
	uint32 adapterIDOverride):
	AdapterIDOverride(adapterIDOverride),
	AdapterID(UINT_MAX),
	BackBufferIndex(0),
	BackBufferFormat(backBufferFormat),
	DepthStencilFormat(depthStencilBufferFormat),
	NumBackBuffer(backBufferCount),
	MinFeatureLevel(minFeatureLevel),
	MaxFeatureLevel(minFeatureLevel),
	VRamSize(0)
{
	if (NumBackBuffer > MAX_BACK_BUFFER_COUNT)
	{
		throw std::out_of_range("Too many back buffers are assigned");
	}

	if (minFeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		throw std::out_of_range("Min feature level is too low");
	}
}

void D3D12DeviceResource::WaitForGPU() noexcept
{
	if (RenderCommandQueue && Fence && FenceEvent.IsValid())
	{
		uint64 fenceValue = FrameResources[BackBufferIndex].FenceValue;
		if (SUCCEEDED(RenderCommandQueue->Signal(Fence.Get(), fenceValue)))
		{
			if (SUCCEEDED(Fence->SetEventOnCompletion(fenceValue, FenceEvent.Get())))
			{
				WaitForSingleObjectEx(FenceEvent.Get(), INFINITE, FALSE);
				++FrameResources[BackBufferIndex].FenceValue;
			}
		}
	}
}

void D3D12DeviceResource::InitDXGIAdapter()
{
	bool debugDXGI = false;
#if defined(_DEBUG) || defined(DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();

		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue))))
		{
			debugDXGI = true;
			ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&Factory4)));

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
	}
#endif
	if (Options)
	{
		BOOL bAllowTearing = FALSE;
		ComPtr<IDXGIFactory5> factory5;
		HRESULT hr = Factory4.As(&factory5);
		if (SUCCEEDED(hr))
		{
			hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bAllowTearing, sizeof(bAllowTearing));
		}
		if (bAllowTearing)
		{
			// TODO: Tearing support
		}
	}

	if (!debugDXGI)
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&Factory4)));
	}

	QueryAdapters();
}

void D3D12DeviceResource::RegisterDeviceNotify(IDeviceNotify* notifier)
{
	DeviceNotify = notifier;
	__if_exists(DXGIDeclareAdapterRemovalSupport)
	{
		if (DeviceNotify)
		{
			if (FAILED(DXGIDeclareAdapterRemovalSupport()))
			{
				ThrowIfFalse(false, "WARNING: Application is failed to declare adapter removal support.");
			}
		}
	}
}

void D3D12DeviceResource::ResizeViewResources()
{
	ViewResources->Resize();
}

void D3D12DeviceResource::BeginFrame(D3D12_RESOURCE_STATES beforeState)
{
	auto Alloc = FrameResources[BackBufferIndex].CommandAllocator.Get();
	ThrowIfFailed(Alloc->Reset());
	ThrowIfFailed(GraphicsCommandList->Reset(Alloc, nullptr));
	if (D3D12_RESOURCE_STATE_RENDER_TARGET != beforeState)
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(ViewResources->GetRenderTarget(BackBufferIndex), beforeState, D3D12_RESOURCE_STATE_RENDER_TARGET);
		GraphicsCommandList->ResourceBarrier(1, &barrier);
	}
}

void D3D12DeviceResource::EndFrame(D3D12_RESOURCE_STATES beforeState)
{
	if (beforeState != D3D12_RESOURCE_STATE_PRESENT)
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(ViewResources->GetRenderTarget(BackBufferIndex), beforeState, D3D12_RESOURCE_STATE_PRESENT);
		GraphicsCommandList->ResourceBarrier(1, &barrier);
	}

	ExecuteCommandLists();

	HRESULT hr = ViewResources->Present();
	if (DXGI_ERROR_DEVICE_REMOVED == hr || DXGI_ERROR_DEVICE_RESET == hr)
	{
		HandleDeviceLost();
		ThrowLastError(false);
	}
	else
	{
		ThrowIfFailed(hr);
		MoveToNextFrame();
	}

	
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DeviceResource::GetRenderTargetView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(ViewResources->RTVHeap->GetCPUDescriptorHandleForHeapStart(), BackBufferIndex, RTVDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12DeviceResource::GetDepthStencilView() const
{
	return ViewResources->DSVHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3D12DeviceResource::ClearRenderTargetView(float color[4])
{
	ViewResources->ClearRenderTargetView(GraphicsCommandList.Get(), color);
}

void D3D12DeviceResource::PrepareGUI()
{
	GraphicsCommandList->SetDescriptorHeaps(1, ViewResources->SRVHeap_GUI.GetAddressOf());
}

void D3D12DeviceResource::QueryAdapters()
{
	IDXGIAdapter1* adapter;
	ComPtr<IDXGIFactory6> factory6;

	ThrowIfFailed(Factory4.As(&factory6));

	for (uint32 adapterId = 0; DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(adapterId, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)); ++adapterId)
	{
		Adapters.push_back(adapter);
		adapter->Release();
	}

	for (size_t i = 0; i < Adapters.size(); ++i)
	{
		if (AdapterIDOverride != UINT_MAX && i != AdapterIDOverride)
		{
			continue;
		}
		DXGI_ADAPTER_DESC1 desc;
		ThrowIfFailed(Adapters[i]->GetDesc1(&desc));

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(Adapters[i].Get(), MinFeatureLevel, _uuidof(ID3D12Device), nullptr)))
		{
			AdapterID = (uint32)i;
			std::wstring d = desc.Description;
			AdapterDescription.resize(d.size());
			std::transform(d.begin(), d.end(), AdapterDescription.begin(), [](wchar_t c) {return (char)c; });

			IDXGIOutput* output = nullptr;
			std::set<uint32> uniqueResolutions;
			std::vector<RECT> coordRects;
			for (uint32 outputIdx = 0; DXGI_ERROR_NOT_FOUND != Adapters[i]->EnumOutputs(outputIdx, &output); ++outputIdx)
			{
				Outputs.push_back(output);
				output->Release();
				output = nullptr;
			}
			VRamSize = desc.DedicatedVideoMemory;
			break;
		}
	}

	FatalIfFalse(AdapterID != UINT_MAX, "Unavailable adapter");
}

void D3D12DeviceResource::CreateDeviceResources()
{
	FatalIfFailed(D3D12CreateDevice(Adapters[AdapterID].Get(), MinFeatureLevel, IID_PPV_ARGS(&Device)));
#if defined(_DEBUG) || defined(DEBUG)
	{
		ComPtr<ID3D12InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(Device.As(&d3dInfoQueue)))
		{
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
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
	}
#endif

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsData =
	{
		_countof(featureLevels), featureLevels, D3D_FEATURE_LEVEL_11_0
	};

	HRESULT hr = Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelsData, sizeof(featureLevelsData));
	if (SUCCEEDED(hr))
	{
		MaxFeatureLevel = featureLevelsData.MaxSupportedFeatureLevel;
	}
	else
	{
		MaxFeatureLevel = MinFeatureLevel;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		FatalIfFailed(Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&RenderCommandQueue)));
	}

	{
		RTVDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		WindowPool* pool = (WindowPool*)ISubsystem::GetSubsystem("WindowPool");
		FatalIfFalse(pool != nullptr, "Window pool is not created yet");
		ViewResources = std::make_unique<D3D12ViewResource>( this, pool->GetWindowFromIndex(0) , true );
	}
	CreateFrameResources();
	FatalIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, FrameResources[0].CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&GraphicsCommandList)));
	ThrowIfFailed(GraphicsCommandList->Close());
	Device->CreateFence(FrameResources[BackBufferIndex].FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence));
	++FrameResources[BackBufferIndex].FenceValue;
	FenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
	FatalAndThrowLastError(FenceEvent.IsValid());
}

void D3D12DeviceResource::CreateFrameResources()
{
	for (uint32 i = 0; i < NumBackBuffer; ++i)
	{
		FatalIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&FrameResources[i].CommandAllocator)));
	}
}

void D3D12DeviceResource::CreateViewDependentResources()
{
	WaitForGPU();
	for (uint32 i = 0; i < NumBackBuffer; ++i)
	{
		FrameResources[i].FenceValue = FrameResources[BackBufferIndex].FenceValue - 1;
	}
	ViewResources->Resize();
	BackBufferIndex = ViewResources->GetCurrentBackBufferIndex();

	MoveToNextFrame();
}

void D3D12DeviceResource::HandleDeviceLost()
{
	if (DeviceNotify)
	{
		DeviceNotify->OnDeviceLost();
	}

	for (uint32 i = 0; i < NumBackBuffer; ++i)
	{
		FrameResources[i].ResetOnDeviceLost();
	}
	
	ViewResources.reset();
	RenderCommandQueue.Reset();
	GraphicsCommandList.Reset();
	Fence.Reset();
	Device.Reset();
	Factory4.Reset();
	for (size_t i = 0; i < Adapters.size(); ++i)
	{
		Adapters[i].Reset();
	}
	for (size_t i = 0; i < Outputs.size(); ++i)
	{
		Outputs[i].Reset();
	}
#if defined(_DEBUG) || defined(DEBUG)
	{
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif

	QueryAdapters();
	CreateDeviceResources();
	CreateViewDependentResources();

	if (DeviceNotify)
	{
		DeviceNotify->OnDeviceRestored();
	}
}

void D3D12DeviceResource::ExecuteCommandLists()
{
	ThrowIfFailed(GraphicsCommandList->Close());
	ID3D12CommandList* lists[] = { GraphicsCommandList.Get()};
	
	RenderCommandQueue->ExecuteCommandLists(ARRAYSIZE(lists), lists);
}

D3D12FrameResource& D3D12DeviceResource::WaitForResource()
{
	return FrameResources[BackBufferIndex];
}

void D3D12DeviceResource::MoveToNextFrame()
{
	const uint64 currentFenceValue = FrameResources[BackBufferIndex].FenceValue;
	ThrowIfFailed(RenderCommandQueue->Signal(Fence.Get(), currentFenceValue));

	BackBufferIndex = ViewResources->GetCurrentBackBufferIndex();
	if (Fence->GetCompletedValue() < FrameResources[BackBufferIndex].FenceValue)
	{
		ThrowIfFailed(Fence->SetEventOnCompletion(FrameResources[BackBufferIndex].FenceValue, FenceEvent.Get()));
		WaitForSingleObjectEx(FenceEvent.Get(), INFINITE, FALSE);
	}

	FrameResources[BackBufferIndex].FenceValue = currentFenceValue + 1;
}
