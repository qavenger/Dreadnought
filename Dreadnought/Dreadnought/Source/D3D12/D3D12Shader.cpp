#include "pch.h"
#include "D3D12Shader.h"
#include "Graphics/VertexFactory.h"

D3D12Shader::D3D12Shader(std::wstring fileName)
{
	uint compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> errorCode;
	HRESULT hr = D3DCompileFromFile(fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_1", compileFlags, 0, &ShaderCode, &errorCode);

	ThrowLastError(errorCode.Get() == nullptr);
}

D3D12VertexShader::D3D12VertexShader(std::wstring fileName)
	:D3D12Shader(fileName)
{
	using namespace VertexFactory;
	ComPtr<ID3D12ShaderReflection> SR;

	D3DReflect(ShaderCode->GetBufferPointer(), ShaderCode->GetBufferSize(), IID_PPV_ARGS(&SR));
	D3D12_SHADER_DESC desc;
	SR->GetDesc(&desc);

	uint flag = 0;
	for (uint i = 0; i < desc.InputParameters; ++i)
	{
		D3D12_SIGNATURE_PARAMETER_DESC d;
		SR->GetInputParameterDesc(i, &d);

		BYTE mask = d.Mask;
		mask = (mask & 0x5) + ((mask >> 1) & 0x5);
		mask = (mask & 0x3) + ((mask >> 2) & 0x3);
		flag |= (1 << GetElementTypeIDByName(d.SemanticName, mask));
	}

	FVertexFactory& vf = FVertexFactory::GetInstance();

	if (flag != 0)
	{
		VertexLayout* layout = nullptr;
		if (vf.GetOrCreateCPUVertexLayout(flag, layout))
		{
			if (layout)
			{
				for (uint i = 0; i < desc.InputParameters; ++i)
				{
					D3D12_SIGNATURE_PARAMETER_DESC d;
					SR->GetInputParameterDesc(i, &d);
					BYTE mask = d.Mask;
					mask = (mask & 0x5) + ((mask >> 1) & 0x5);
					mask = (mask & 0x3) + ((mask >> 2) & 0x3);
					layout->AppendElement((ElementType)GetElementTypeIDByName(d.SemanticName, mask), d.SemanticIndex);
				}
				pInputLayout = vf.Finalize(flag);
			}
		}
		else
		{
			if (layout)
			{
				pInputLayout = vf.GetGPUVertexLayout(flag);
			}
		}
		
	}

	int i = 0;
}
