#include "stdafx.h"
#include "RenderTarget.h"

RHITexture* RHIRenderTarget::GetTexture() const
{
	return Texture;
}