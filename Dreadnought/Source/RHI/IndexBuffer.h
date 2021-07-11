#pragma once

class IIndexBuffer
{
public:
	void SetData(std::vector<uint32>& Data);
	void SetData(std::vector<uint16>& Data);

	uint32 GetTriangleCount() const;
	uint32 GetIndexCount() const;
	uint32 GetDataSize() const;
	void* GetData() const;

protected:
	uint32                            SizeInBytes;
	uint32                            IndexCount;
	std::vector<uint16>               IndexData16;
	std::vector<uint32>               IndexData32;
	bool                              IsHalf;
};