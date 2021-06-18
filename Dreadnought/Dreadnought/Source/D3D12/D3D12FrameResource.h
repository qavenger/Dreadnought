#pragma once
class D3D12FrameResource
{
	friend class D3D12DeviceResource;
private:
	D3D12FrameResource();
	void ResetWindowDependentResources();
	void ResetOnDeviceLost();
private:
	ComPtr<ID3D12CommandAllocator>	CommandAllocator;
	ComPtr<ID3D12Resource>			RenderTarget;
	UINT64							FenceValue;
};

