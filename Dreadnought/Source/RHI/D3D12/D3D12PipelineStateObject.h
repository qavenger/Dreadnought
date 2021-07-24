#pragma once

#include "../PipelineStateObject.h"

class D3D12PipelineStateObject : public RHIPipelineStateObject
{
public:
	virtual void Init();

public:
	ComPtr<ID3D12PipelineState>& GetPSO();
	ComPtr<ID3D12RootSignature> GetRootSignature() const;
	const D3D12_INPUT_ELEMENT_DESC* GetInputLayout() const;
	uint32 GetInputLayoutSize() const;

private:
	ComPtr<ID3D12PipelineState>           PSO;
	ComPtr<ID3D12RootSignature>           RootSignature;
	std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
	std::vector<std::string>              SemanticName;
};