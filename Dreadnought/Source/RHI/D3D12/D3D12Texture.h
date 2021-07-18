#pragma once

#include "../Texture.h"

class D3D12Texture : public RHITexture
{
public:
	D3D12Texture();

	ComPtr<ID3D12Resource>& GetResource() { return Resource; }

public:
	virtual void* GetResourceRaw() { return Resource.Get(); }

private:
	ComPtr<ID3D12Resource> Resource;
};