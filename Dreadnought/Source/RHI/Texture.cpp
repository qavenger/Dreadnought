#include "stdafx.h"
#include "Texture.h"

RHITexture::RHITexture()
{

}

void RHITexture::SetDesc(const TextureDesc& Desc)
{
	this->Desc = Desc;
}


TextureDesc RHITexture::GetDesc() const
{
	return Desc;
}