#pragma once
#include "D3D12FrameResource.h"
#include "D3D12ViewResource.h"

interface IDeviceNotify
{
	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceRestored() = 0;
};
class D3D12DeviceResource
{
	friend class RHIDevice;
public:
	D3D12DeviceResource(
		DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT depthStencilBufferFormat = DXGI_FORMAT_D32_FLOAT,
		uint32 backBufferCount = 2,
		D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_11_0,
		uint32 flags = 0,
		uint32 adapterIDOverride = UINT_MAX
	);
	virtual ~D3D12DeviceResource() { WaitForGPU(); }
public:
	void WaitForGPU()noexcept;
	void InitDXGIAdapter();
	void RegisterDeviceNotify(IDeviceNotify* notifier);
	void ResizeViewResources();
public:
	uint32 GetBackBufferCount()const { return NumBackBuffer; }
	DXGI_FORMAT GetBackBufferFormat()const { return BackBufferFormat; }
	DXGI_FORMAT GetDepthStencilFormat()const { return DepthStencilFormat; }
	ID3D12CommandQueue* GetRenderCommandQueue()const { return RenderCommandQueue.Get(); }
	IDXGIFactory4* GetIDXGIFactory()const { return Factory4.Get(); }
	ID3D12Device* GetDevice()const { return Device.Get(); }
	uint32 GetRTVDescriptorSize()const { return RTVDescriptorSize; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()const;
	void ClearRenderTargetView(float color[4]);
	void PrepareGUI();
private:
	void QueryAdapters();
	void CreateDeviceResources();
	void CreateFrameResources();
	void CreateViewDependentResources();
	void HandleDeviceLost();
	void ExecuteCommandLists();
	void BeginFrame(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT);
	void EndFrame(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET);
private:
	D3D12FrameResource& WaitForResource();
	void MoveToNextFrame();
private:
	uint32 AdapterID;
	uint32 AdapterIDOverride;
	uint32 BackBufferIndex;
	std::vector<ComPtr<IDXGIAdapter1>>			Adapters;
	std::vector<ComPtr<IDXGIOutput>>			Outputs;
	ComPtr<ID3D12Device>						Device;
	ComPtr<ID3D12CommandQueue>					RenderCommandQueue;
	ComPtr<ID3D12GraphicsCommandList>			GraphicsCommandList;

	ComPtr<IDXGIFactory4>						Factory4;
	ComPtr<ID3D12Fence>							Fence;
	Microsoft::WRL::Wrappers::Event				FenceEvent;

	D3D12FrameResource							FrameResources[MAX_BACK_BUFFER_COUNT];
	std::unique_ptr<D3D12ViewResource>			ViewResources;

	uint32										RTVDescriptorSize = -1;
	uint32										Options = 0;
	DXGI_FORMAT									BackBufferFormat;
	DXGI_FORMAT									DepthStencilFormat;
	uint32										NumBackBuffer;
	SIZE_T										VRamSize;
	D3D_FEATURE_LEVEL							MinFeatureLevel;
	D3D_FEATURE_LEVEL							MaxFeatureLevel;

	IDeviceNotify*								DeviceNotify = nullptr;
	std::string									AdapterDescription;
};

