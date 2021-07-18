#pragma once

#include "Texture.h"

class RHIRenderTarget
{
public:
	virtual ~RHIRenderTarget() {}

	RHITexture* GetTexture() const;

protected:
	RHITexture*          Texture;
};