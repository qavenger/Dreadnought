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

class ITexture
{
public:
	ITexture(TextureDesc& Desc);
	virtual ~ITexture() {}
	
	TextureDesc GetDesc() const { return Desc; }

private:
	TextureDesc Desc;
};