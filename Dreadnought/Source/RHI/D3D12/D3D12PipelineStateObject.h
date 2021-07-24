#pragma once

#include "../PipelineStateObject.h"

class D3D12PipelineStateObject : public RHIPipelineStateObject
{
public:
	virtual void Init();

public:
	ComPtr<ID3D12RootSignature>& GetRootSignature();
	ComPtr<ID3D12PipelineState>& GetPSO();

private:
	ComPtr<ID3D12RootSignature> RootSignature;
	ComPtr<ID3D12PipelineState> PSO;
};