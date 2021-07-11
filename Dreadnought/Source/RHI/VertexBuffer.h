#pragma once

class IVertexBuffer
{
public:
	virtual ~IVertexBuffer() {}

	void SetData(uint32 Count, std::vector<float>& Data, std::vector<ETextureFormat>& Layout);

	uint32 GetSizePerVertex() const;
	uint32 GetVertexCount() const;
	uint32 GetDataSize() const;
	void* GetData() const;

protected:
	uint32                            SizeInBytes;
	uint32                            VertexCount;
	std::vector<ETextureFormat>        VertexLayout;
	std::vector<float>                VertexData;
};