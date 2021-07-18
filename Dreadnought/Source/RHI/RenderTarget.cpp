#include "stdafx.h"
#include "RenderTarget.h"

void RHIRenderTarget::SetTexture(RHITexture* Texture)
{
	this->Texture = Texture;
}

RHITexture* RHIRenderTarget::GetTexture() const
{
	return Texture;
}