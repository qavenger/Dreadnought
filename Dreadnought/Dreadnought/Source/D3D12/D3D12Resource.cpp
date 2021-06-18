#include "pch.h"
#include "D3D12Resource.h"

uint64 ID3D12ResourcePool::PoolSize;

ComPtr<ID3D12Resource> ID3D12ResourceUtil::CreateBuffer(ID3D12GraphicsCommandList* cmdList, ID3D12Device* device, const void* initData, uint64 bufferSize, D3D12_HEAP_TYPE type /*= D3D12_HEAP_TYPE_DEFAULT*/)
{
	ComPtr<ID3D12Resource> createdBuffer;
	const D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(type);
	const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	ThrowIfFailed(device->CreateCommittedResource(
		&defaultHeapProperties
		, D3D12_HEAP_FLAG_NONE
		, &desc
		, D3D12_RESOURCE_STATE_GENERIC_READ
		, nullptr
		, IID_PPV_ARGS(&createdBuffer)));

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = bufferSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	if (type == D3D12_HEAP_TYPE_DEFAULT)
	{
		const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		ComPtr<ID3D12Resource> uploadBuffer;
		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProperties
			, D3D12_HEAP_FLAG_NONE
			, &desc
			, D3D12_RESOURCE_STATE_GENERIC_READ
			, nullptr
			, IID_PPV_ARGS(&uploadBuffer)));
		const D3D12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(createdBuffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
		cmdList->ResourceBarrier(1, &transition);
		UpdateSubresources<1>(cmdList, createdBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
	}
	else
	{
		void* data = nullptr;
		createdBuffer->Map(0, nullptr, &data);
		memcpy(data, initData, bufferSize);
		if (createdBuffer)createdBuffer->Unmap(0, nullptr);
		data = nullptr;
	}

	const D3D12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(createdBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	cmdList->ResourceBarrier(1, &transition);


	return createdBuffer;
}

void ID3D12ResourcePool::Init(uint64 poolSize)
{
	PoolSize = poolSize;
}