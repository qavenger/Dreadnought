#include <stdafx.h>
#include "D3D12RenderResource.h"

//std::vector<D3D12RenderResource*> D3D12RenderResourcePool::RenderResources;
//std::vector<uint32> D3D12RenderResourcePool::EmptySlots;
//std::map<D3D12RenderResource*, uint32> D3D12RenderResourcePool::ResourcesLookup;

//D3D12RenderResource::D3D12RenderResource()
//{
//	if (EmptySlots.empty())
//	{
//		ResourcesLookup[this] = (uint32)RenderResources.size();
//		RenderResources.push_back(this);
//	}
//	else
//	{
//		uint32 i = EmptySlots.back();
//		RenderResources[i]= this;
//		ResourcesLookup[this] = i;
//		EmptySlots.pop_back();
//	}
//}
//
//D3D12RenderResource::~D3D12RenderResource()
//{
//	EmptySlots.push_back(ResourcesLookup[this]);
//	RenderResources[ResourcesLookup[this]] = nullptr;
//	ResourcesLookup.erase(this);
//}
