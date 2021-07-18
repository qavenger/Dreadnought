#pragma once

#include "Texture.h"

class RHIRenderTarget
{
public:
	virtual ~RHIRenderTarget() { delete Texture; }

	void SetTexture(RHITexture* Texture);

	RHITexture* GetTexture() const;

protected:
	RHITexture*          Texture;
};