#include <stdafx.h>
#include "D3D12VertexBuffer.h"

void D3D12VertexBuffer::CreateVertexBuffer() const
{

}

D3D12_VERTEX_BUFFER_VIEW D3D12VertexBuffer::GetVertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW View;
	View.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
	View.SizeInBytes = SizeInBytes;
	View.StrideInBytes = GetSizePerVertex();

	return View;
}