#pragma once

#include "Texture.h"

class IRenderTarget
{
public:
	virtual ~IRenderTarget() {}

	ITexture* GetTexture() const;

protected:
	ITexture*          Texture;
};