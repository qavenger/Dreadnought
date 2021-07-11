#pragma once

#include "../Texture.h"

class D3D12Texture : public ITexture
{
public:
	D3D12Texture(TextureDesc& Desc);

	ComPtr<ID3D12Resource> GetResource() { return Resource; }

private:
	ComPtr<ID3D12Resource> Resource;
};