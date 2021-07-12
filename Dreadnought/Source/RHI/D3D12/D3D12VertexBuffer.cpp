#include <stdafx.h>
#include "D3D12VertexBuffer.h"

void* D3D12VertexBuffer::GetVertexLayout()
{
	VertexLayout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 } };

	return (void*)&VertexLayout[0];
}

D3D12_VERTEX_BUFFER_VIEW D3D12VertexBuffer::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW View;
	View.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
	View.SizeInBytes = SizeInBytes;
	View.StrideInBytes = GetSizePerVertex();

	return View;
}