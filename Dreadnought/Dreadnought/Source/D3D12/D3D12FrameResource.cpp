#include "pch.h"
#include "D3D12FrameResource.h"

D3D12FrameResource::D3D12FrameResource()
	:
	FenceValue(0)
{
}

void D3D12FrameResource::ResetWindowDependentResources()
{
	RenderTarget.Reset();
}

void D3D12FrameResource::ResetOnDeviceLost()
{
	CommandAllocator.Reset();
	RenderTarget.Reset();
}
