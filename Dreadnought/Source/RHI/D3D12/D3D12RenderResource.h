#pragma once
//class D3D12RenderResource
//{
//public:
//	D3D12RenderResource();
//	virtual ~D3D12RenderResource();
//	virtual void Reset() = 0;
//private:
//#if defined(_DEBUG) || defined(DEBUG)
//	std::wstring Name;
//#endif
//
//};
//
//class D3D12RenderResourcePool
//{
//	friend class D3D12RenderResource;
//private:
//	static std::vector<D3D12RenderResource*> RenderResources;
//	static std::vector<uint32> EmptySlots;
//	static std::map<D3D12RenderResource*, uint32> ResourcesLookup;
//};
//
//class D3D12BindableResource : public D3D12RenderResource
//{
//	virtual void Reset() { Resource.Reset(); }
//private:
//	ComPtr<ID3D12Resource> Resource;
//};
//
//class D3D12VertexBuffer : public D3D12BindableResource
//{
//
//};
//
//class D3D12Texture2D : public D3D12BindableResource
//{
//};
//
//class D3D12RenderTarget : public D3D12BindableResource
//{
//	
//};