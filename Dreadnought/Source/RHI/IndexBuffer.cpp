#include "stdafx.h"
#include "IndexBuffer.h"


void IIndexBuffer::SetData(std::vector<uint32>& Data)
{
	IndexCount = Data.size();
	SizeInBytes = IndexCount * sizeof(uint32);
	IsHalf = false;

	IndexData32 = Data;
}

void IIndexBuffer::SetData(std::vector<uint16>& Data)
{
	IndexCount = Data.size();
	SizeInBytes = IndexCount * sizeof(uint16);
	IsHalf = true;

	IndexData16 = Data;
}

uint32 IIndexBuffer::GetTriangleCount() const 
{
	return IndexCount / 3;
}

uint32 IIndexBuffer::GetIndexCount() const
{
	return IndexCount;
}

uint32 IIndexBuffer::GetDataSize() const
{
	return SizeInBytes;
}

void* IIndexBuffer::GetData() const
{
	return IsHalf ? (void*)&IndexData16[0] : (void*)&IndexData32[0];
}