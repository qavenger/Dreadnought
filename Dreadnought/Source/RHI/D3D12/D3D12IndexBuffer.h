#pragma once

#include "../IndexBuffer.h"

class D3D12IndexBuffer : public IIndexBuffer
{
public:
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
public:
	ComPtr<ID3D12Resource>         IndexBufferGPU;
	ComPtr<ID3D12Resource>         UploadBuffer;
};