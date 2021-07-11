#include <stdafx.h>
#include "D3D12Shader.h"
#include "Engine/Rendering/VertexFactory.h"


ComPtr<ID3DBlob>& D3D12Shader::GetShaderCode()
{
	return ShaderCode;
}
//
//void D3D12Shader::Reset()
//{
//	ShaderCode.Reset();
//}
//
//D3D12VertexShader::D3D12VertexShader(std::wstring fileName)
//	:D3D12Shader(fileName)
//{
//	ComPtr<ID3D12ShaderReflection> SR;
//
//	D3DReflect(ShaderCode->GetBufferPointer(), ShaderCode->GetBufferSize(), IID_PPV_ARGS(&SR));
//	D3D12_SHADER_DESC desc;
//	SR->GetDesc(&desc);
//
//	uint32 flag = 0;
//	for (uint32 i = 0; i < desc.InputParameters; ++i)
//	{
//		D3D12_SIGNATURE_PARAMETER_DESC d;
//		SR->GetInputParameterDesc(i, &d);
//
//		BYTE mask = d.Mask;
//		mask = (mask & 0x5) + ((mask >> 1) & 0x5);
//		mask = (mask & 0x3) + ((mask >> 2) & 0x3);
//		flag |= (1 << GetElementTypeIDByName(d.SemanticName, mask));
//	}
//
//	/*VertexFactory& vf = VertexFactory::GetInstance();
//
//	if (flag != 0)
//	{
//		VertexLayout* layout = nullptr;
//		if (vf.GetOrCreateCPUVertexLayout(flag, layout))
//		{
//			if (layout)
//			{
//				for (uint32 i = 0; i < desc.InputParameters; ++i)
//				{
//					D3D12_SIGNATURE_PARAMETER_DESC d;
//					SR->GetInputParameterDesc(i, &d);
//					BYTE mask = d.Mask;
//					mask = (mask & 0x5) + ((mask >> 1) & 0x5);
//					mask = (mask & 0x3) + ((mask >> 2) & 0x3);
//					layout->AppendElement((ElementType)GetElementTypeIDByName(d.SemanticName, mask), d.SemanticIndex);
//				}
//				pInputLayout = vf.Finalize(flag);
//			}
//		}
//		else
//		{
//			if (layout)
//			{
//				pInputLayout = vf.GetGPUVertexLayout(flag);
//			}
//		}
//		
//	}*/
//
//}
