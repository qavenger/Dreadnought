#pragma once

#include "../ConstantBuffer.h"

class D3D12ConstantBuffer : public RHIConstantBuffer
{
public:

	virtual void UpdateData(void* Data, uint32 Size, uint32 Base) const;

public:
	ComPtr<ID3D12Resource>& GetResource();

private:
	ComPtr<ID3D12Resource>   UploadBuffer;
};
