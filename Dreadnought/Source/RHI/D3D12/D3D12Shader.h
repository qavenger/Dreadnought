#pragma once
#include "../Shader.h"

class D3D12Shader : public RHIShader
{
public:
	virtual ~D3D12Shader() {}

	ComPtr<ID3DBlob>& GetShaderCode();

protected:
	ComPtr<ID3DBlob> ShaderCode;
};