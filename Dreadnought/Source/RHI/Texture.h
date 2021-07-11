#pragma once

//Clear Color/Depht/Stencil Value
struct ClearCDSValue
{
	float               ClearColorR;
	float               ClearColorG;
	float               ClearColorB;
	float               ClearColorA;
	float               ClearDepth;
	uint8               ClearStencil;

	ClearCDSValue()
		:ClearColorR(0.f)
		,ClearColorG(0.f)
		,ClearColorB(0.f)
		,ClearColorA(1.f)
		,ClearDepth(1.f)
		,ClearStencil(0)
	{

	}
};

struct TextureDesc
{
	ETextureDimension    Dimension;
	uint32               TextureWidth;
	uint32               TextureHeight;
	uint32               TextureDepth;
	uint32               MipmapLevels;
	ETextureFormat       Format;
	ClearCDSValue        ClearValues;
	bool                 ClearValid = false;

	bool IsClearValid() const { return ClearValid; }
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