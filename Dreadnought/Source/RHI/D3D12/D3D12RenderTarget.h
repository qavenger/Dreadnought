#pragma once

#include "../RenderTarget.h"

class D3D12RenderTarget : public IRenderTarget
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;

private:
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView;
};