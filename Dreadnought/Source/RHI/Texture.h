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
	RHITexture(TextureDesc& Desc);
	virtual ~RHITexture() {}
	
	TextureDesc GetDesc() const { return Desc; }

private:
	TextureDesc Desc;
};