#include "stdafx.h"
#include "D3D12ConstantBuffer.h"

void D3D12ConstantBuffer::UpdateData(void* Data, uint32 Size, uint32 Base) const
{
	BYTE* DataPtr = nullptr;
	ThrowIfFailed(UploadBuffer->Map(0, nullptr, (void**)&DataPtr));

	memcpy(&DataPtr[Base * ElementSize], Data, Size);

	UploadBuffer->Unmap(0, nullptr);
}

ComPtr<ID3D12Resource>& D3D12ConstantBuffer::GetResource()
{
	return UploadBuffer;
}