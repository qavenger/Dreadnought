#pragma once
#include "D3D12RenderResource.h"
class D3D12Shader /*: public D3D12RenderResource*/
{
public:
	D3D12Shader(std::wstring fileName);
public:
	// Inherited via D3D12RenderResource
	virtual void Reset();
protected:
	ComPtr<ID3DBlob> ShaderCode;
};

class D3D12VertexShader : public D3D12Shader
{
public:
	D3D12VertexShader(std::wstring fileName);
private:
	const std::vector<D3D12_INPUT_ELEMENT_DESC>* pInputLayout = nullptr;
};