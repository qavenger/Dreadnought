#include "stdafx.h"
#include "RenderTarget.h"

ITexture* IRenderTarget::GetTexture() const
{
	return Texture;
}