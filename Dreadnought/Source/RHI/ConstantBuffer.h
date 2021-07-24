#pragma once

class RHIConstantBuffer
{
public:
	virtual ~RHIConstantBuffer() {}

	virtual void UpdateData(void* Data, uint32 Size, uint32 Base) const = 0;

public:
	void SetElementCount(uint32 Count);
	void SetElementSize(uint32 Size);
	uint32 GetElementCount() const;
	uint32 GetElementSize() const;

protected:
	uint32 ElementCount;
	uint32 ElementSize;
};