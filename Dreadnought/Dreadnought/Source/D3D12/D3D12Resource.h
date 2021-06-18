#pragma once
class ID3D12ResourcePool : public INoncopyable
{
public:
	static void Init(uint64 poolSize);
private:
	static uint64 PoolSize;
};

class ID3D12ResourceUtil : public INoncopyable
{
public:
	static ComPtr<ID3D12Resource> CreateBuffer(ID3D12GraphicsCommandList* cmdList, ID3D12Device* device, const void* initData, uint64 bufferSize, D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT);
};