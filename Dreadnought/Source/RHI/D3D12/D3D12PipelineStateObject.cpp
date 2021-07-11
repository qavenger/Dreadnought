#include "stdafx.h"
#include "D3D12PipelineStateObject.h"


ComPtr<ID3D12PipelineState> D3D12PipelineStateObject::GetPSO()
{
	return PSO;
}