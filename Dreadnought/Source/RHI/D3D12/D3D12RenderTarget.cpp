#include "stdafx.h"
#include "D3D12RenderTarget.h"


D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderTarget::GetRenderTargetView() const
{
	return RenderTargetView;
}