#include "stdafx.h"
#include "ConstantBuffer.h"

void RHIConstantBuffer::SetElementCount(uint32 Count)
{
	this->ElementCount = Count;
}
void RHIConstantBuffer::SetElementSize(uint32 Size)
{
	this->ElementSize = Size;
}

uint32 RHIConstantBuffer::GetElementCount() const
{
	return ElementCount;
}

uint32 RHIConstantBuffer::GetElementSize() const
{
	return ElementSize;
}