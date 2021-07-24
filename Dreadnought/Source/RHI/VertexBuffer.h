#pragma once

class RHIVertexBuffer
{
public:
	virtual ~RHIVertexBuffer() {}

public:
	void SetData(uint32 Count, std::vector<float>& Data);

	uint32 GetSizePerVertex() const;
	uint32 GetVertexCount() const;
	uint32 GetDataSize() const;
	void* GetData() const;

protected:
	uint32                            SizeInBytes;
	uint32                            VertexCount;
	std::vector<float>                VertexData;
};