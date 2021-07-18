#include "stdafx.h"
#include "VertexBuffer.h"

void RHIVertexBuffer::SetData(uint32 Count, std::vector<float>& Data, std::vector<ETextureFormat>& Layout)
{
	VertexCount = Count;
	SizeInBytes = (uint32)Data.size() * sizeof(float);

	VertexData = Data;
	VertexLayout = Layout;
}

uint32 RHIVertexBuffer::GetSizePerVertex() const
{
	return SizeInBytes / VertexCount;
}

uint32 RHIVertexBuffer::GetVertexCount() const
{
	return VertexCount;
}

uint32 RHIVertexBuffer::GetDataSize() const
{
	return SizeInBytes;
}

void* RHIVertexBuffer::GetData() const
{
	return (void*)&VertexData[0];
}