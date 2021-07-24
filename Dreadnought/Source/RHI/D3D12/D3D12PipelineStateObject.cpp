#include "stdafx.h"
#include "D3D12PipelineStateObject.h"
#include "D3D12Shader.h"
#include "D3D12Device.h"

void D3D12PipelineStateObject::Init()
{
	gDevice->BuildShader(VertexShader);
	gDevice->BuildShader(PixelShader);

	D3D12Shader* VS = (D3D12Shader*)VertexShader;
	D3D12Shader* PS = (D3D12Shader*)PixelShader;
	ComPtr<ID3D12ShaderReflection> CodeReflection;
	D3DReflect(PS->GetShaderCode()->GetBufferPointer(), PS->GetShaderCode()->GetBufferSize(), IID_PPV_ARGS(&CodeReflection));

	D3D12_SHADER_DESC ShaderDesc;
	CodeReflection->GetDesc(&ShaderDesc);
	const uint32 NumConstantBuffer = ShaderDesc.ConstantBuffers;
	std::vector<CD3DX12_ROOT_PARAMETER> RootParameters(NumConstantBuffer);
	for (uint32 Index = 0; Index < NumConstantBuffer; ++Index)
	{
		ID3D12ShaderReflectionConstantBuffer* CB = CodeReflection->GetConstantBufferByIndex(Index);
		D3D12_SHADER_BUFFER_DESC ShaderBufferDesc;
		CB->GetDesc(&ShaderBufferDesc);
		RootParameters[Index].InitAsConstantBufferView(Index);
	}

	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	RootSignatureDesc.NumParameters = NumConstantBuffer;
	RootSignatureDesc.pParameters = &RootParameters[0];
	RootSignatureDesc.NumStaticSamplers = 0;
	RootSignatureDesc.pStaticSamplers = nullptr;
	RootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	ComPtr<ID3DBlob> SerializedRootSig = nullptr;
	ComPtr<ID3DBlob> ErrorBlob = nullptr;
	HRESULT Result = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &SerializedRootSig, &ErrorBlob);
	if (ErrorBlob != nullptr)
	{
		::OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
	}
	ThrowIfFailed(Result);

	RootSignature = ((D3D12Device*)gDevice)->CreateRootSignature(SerializedRootSig);
}

ComPtr<ID3D12RootSignature>& D3D12PipelineStateObject::GetRootSignature()
{
	return RootSignature;
}

ComPtr<ID3D12PipelineState>& D3D12PipelineStateObject::GetPSO()
{
	return PSO;
}