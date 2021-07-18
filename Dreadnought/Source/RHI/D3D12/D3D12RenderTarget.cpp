#include "stdafx.h"
#include "D3D12RenderTarget.h"

void D3D12RenderTarget::SetRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE Handle)
{
	RenderTargetView = Handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderTarget::GetRenderTargetView() const
{
	return RenderTargetView;
}