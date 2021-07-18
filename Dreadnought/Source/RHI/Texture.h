#pragma once

struct TextureDesc
{
	ETextureDimension    Dimension;
	uint32               TextureWidth;
	uint32               TextureHeight;
	uint32               TextureDepth;
	uint32               MipmapLevels;
	ETextureFormat       Format;
	std::wstring         Name;
	bool                 UseForDepth;
};

class RHITexture
{
public:
	RHITexture();
	virtual ~RHITexture() {}
	
	void SetDesc(const TextureDesc& Desc);

	TextureDesc GetDesc() const;

public:
	// return raw resource pointer, such as ID3D12Resouce*
	virtual void* GetResourceRaw() = 0;

private:
	TextureDesc Desc;
};