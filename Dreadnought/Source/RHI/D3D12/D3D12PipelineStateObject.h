#pragma once

#include "../PipelineStateObject.h"

class D3D12PipelineStateObject : public IPipelineStateObject
{
public:

	ComPtr<ID3D12PipelineState> GetPSO();

private:
	ComPtr<ID3D12PipelineState> PSO;
};