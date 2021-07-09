#pragma once
class D3D12FrameResource
{
	friend class D3D12DeviceResource;
private:
	void ResetOnDeviceLost();
private:
	ComPtr<ID3D12CommandAllocator>				CommandAllocator; 
	uint64										FenceValue = 0;
};

