#pragma once
class D3D12Shader
{
public:
	D3D12Shader(std::wstring fileName);
protected:
	ComPtr<ID3DBlob> ShaderCode;
};

class D3D12VertexShader : public D3D12Shader
{
public:
	D3D12VertexShader(std::wstring fileName);
private:
	const std::vector<D3D12_INPUT_ELEMENT_DESC>* pInputLayout;
};