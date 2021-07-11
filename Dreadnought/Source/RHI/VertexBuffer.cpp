#include "stdafx.h"
#include "VertexBuffer.h"

void IVertexBuffer::SetData(uint32 Count, std::vector<float>& Data, std::vector<ETextureFormat>& Layout)
{
	VertexCount = Count;
	SizeInBytes = Data.size() * sizeof(float);

	VertexData = Data;
	VertexLayout = Layout;
}

uint32 IVertexBuffer::GetSizePerVertex() const
{
	return SizeInBytes / VertexCount;
}

uint32 IVertexBuffer::GetVertexCount() const
{
	return VertexCount;
}

uint32 IVertexBuffer::GetDataSize() const
{
	return SizeInBytes;
}

void* IVertexBuffer::GetData() const
{
	return (void*)&VertexData[0];
}