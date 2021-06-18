#pragma once
class D3D12Texture
{
public:
	static ID3D12Resource* Create(ID3D12Device* Device);
private:
	ComPtr<ID3D12Resource> Resource;
};

