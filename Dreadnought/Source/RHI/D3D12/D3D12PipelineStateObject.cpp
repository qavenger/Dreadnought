#include "stdafx.h"
#include "D3D12PipelineStateObject.h"
#include "D3D12Shader.h"
#include "D3D12Device.h"


D3D12PipelineStateObject::D3D12PipelineStateObject()
	:NumShaderConstantBuffers(0)
{

}

void D3D12PipelineStateObject::Init()
{
	gDevice->BuildShader(VertexShader);
	gDevice->BuildShader(PixelShader);

	D3D12Shader* VS = (D3D12Shader*)VertexShader;
	D3D12Shader* PS = (D3D12Shader*)PixelShader;

	ComPtr<ID3D12ShaderReflection> VSCodeReflection;
	D3DReflect(VS->GetShaderCode()->GetBufferPointer(), VS->GetShaderCode()->GetBufferSize(), IID_PPV_ARGS(&VSCodeReflection));
	ComPtr<ID3D12ShaderReflection> PSCodeReflection;
	D3DReflect(PS->GetShaderCode()->GetBufferPointer(), PS->GetShaderCode()->GetBufferSize(), IID_PPV_ARGS(&PSCodeReflection));

	static DXGI_FORMAT InputFormatMat[3][4] =
	{
		{DXGI_FORMAT_R32_UINT,  DXGI_FORMAT_R32G32_UINT,  DXGI_FORMAT_R32G32B32_UINT,  DXGI_FORMAT_R32G32B32A32_UINT},
		{DXGI_FORMAT_R32_SINT,  DXGI_FORMAT_R32G32_SINT,  DXGI_FORMAT_R32G32B32_SINT,  DXGI_FORMAT_R32G32B32A32_SINT},
		{DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT}
	};

	//Build Input Layout
	D3D12_SHADER_DESC VertexShaderDesc;
	VSCodeReflection->GetDesc(&VertexShaderDesc);
	const uint32 NumInput = VertexShaderDesc.InputParameters;
	InputLayout.resize(NumInput);
	SemanticName.resize(NumInput);
	uint32 Stride = 0;
	for (uint32 Index = 0; Index < NumInput; ++Index)
	{
		D3D12_SIGNATURE_PARAMETER_DESC SignatureParameterDesc;
		VSCodeReflection->GetInputParameterDesc(Index, &SignatureParameterDesc);
		D3D_REGISTER_COMPONENT_TYPE ComponentType = SignatureParameterDesc.ComponentType;
		uint8 Mask = SignatureParameterDesc.Mask;
		uint32 NumComponent = (Mask & 0x1) + ((Mask & 0x2) >> 1) + ((Mask & 0x4) >> 2) + ((Mask & 0x8) >> 3);

		//record semantic name before be released
		SemanticName[Index] = SignatureParameterDesc.SemanticName;
		InputLayout[Index].SemanticName = SemanticName[Index].c_str();
		InputLayout[Index].SemanticIndex = SignatureParameterDesc.SemanticIndex;
		InputLayout[Index].Format = InputFormatMat[(uint32)ComponentType - 1][NumComponent - 1];
		InputLayout[Index].InputSlot = 0;
		InputLayout[Index].AlignedByteOffset = Stride;
		InputLayout[Index].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		InputLayout[Index].InstanceDataStepRate = 0;
		
		Stride += NumComponent * 4;
	}

	//Build Root Signature
	D3D12_SHADER_DESC PixelShaderDesc;
	PSCodeReflection->GetDesc(&PixelShaderDesc);
	NumShaderConstantBuffers = PixelShaderDesc.ConstantBuffers;
	std::vector<CD3DX12_ROOT_PARAMETER> RootParameters(NumShaderConstantBuffers);
	for (uint32 Index = 0; Index < NumShaderConstantBuffers; ++Index)
	{
		ID3D12ShaderReflectionConstantBuffer* CB = PSCodeReflection->GetConstantBufferByIndex(Index);
		D3D12_SHADER_BUFFER_DESC ShaderBufferDesc;
		CB->GetDesc(&ShaderBufferDesc);
		RootParameters[Index].InitAsConstantBufferView(Index);
	}

	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
	RootSignatureDesc.NumParameters = NumShaderConstantBuffers;
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

	gDevice->BuildPipelineStateObject(this);
}

ComPtr<ID3D12PipelineState>& D3D12PipelineStateObject::GetPSO()
{
	return PSO;
}

ComPtr<ID3D12RootSignature> D3D12PipelineStateObject::GetRootSignature() const
{
	return RootSignature;
}

const D3D12_INPUT_ELEMENT_DESC* D3D12PipelineStateObject::GetInputLayout() const
{
	return &InputLayout[0];
}

uint32 D3D12PipelineStateObject::GetInputLayoutSize() const
{
	return (uint32)InputLayout.size();
}

uint32 D3D12PipelineStateObject::GetShaderConstantBuffersCount() const
{
	return NumShaderConstantBuffers;
}