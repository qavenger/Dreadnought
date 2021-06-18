#pragma once
class D3D12Shader
{
public:
	D3D12Shader(std::wstring fileName);
private:
	ComPtr<ID3DBlob> ShaderCode;
};

