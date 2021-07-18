#pragma once

struct TextureDesc
{
	ETextureDimension    Dimension;
	uint32               TextureWidth;
	uint32               TextureHeight;
	uint32               TextureDepth;
	uint32               MipmapLevels;
	ETextureFormat       Format;
};

class RHITexture
{
public:
	RHITexture();
	virtual ~RHITexture() {}
	
	void SetDesc(const TextureDesc& Desc);

	TextureDesc GetDesc() const;

private:
	TextureDesc Desc;
};