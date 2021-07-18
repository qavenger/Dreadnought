#include "stdafx.h"
#include "IndexBuffer.h"


void RHIIndexBuffer::SetData(std::vector<uint32>& Data)
{
	IndexCount = (uint32)Data.size();
	SizeInBytes = IndexCount * sizeof(uint32);
	IsHalf = false;

	IndexData32 = Data;
}

void RHIIndexBuffer::SetData(std::vector<uint16>& Data)
{
	IndexCount = (uint32)Data.size();
	SizeInBytes = IndexCount * sizeof(uint16);
	IsHalf = true;

	IndexData16 = Data;
}

uint32 RHIIndexBuffer::GetTriangleCount() const 
{
	return IndexCount / 3;
}

uint32 RHIIndexBuffer::GetIndexCount() const
{
	return IndexCount;
}

uint32 RHIIndexBuffer::GetDataSize() const
{
	return SizeInBytes;
}

void* RHIIndexBuffer::GetData() const
{
	return IsHalf ? (void*)&IndexData16[0] : (void*)&IndexData32[0];
}