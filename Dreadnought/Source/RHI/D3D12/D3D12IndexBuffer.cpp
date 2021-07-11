#include "stdafx.h"
#include "D3D12IndexBuffer.h"

D3D12_INDEX_BUFFER_VIEW D3D12IndexBuffer::GetIndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW View;
	View.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
	View.SizeInBytes = SizeInBytes;
	View.Format = IsHalf ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	return View;
}