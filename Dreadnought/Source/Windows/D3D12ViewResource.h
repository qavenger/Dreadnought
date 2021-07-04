#pragma once
class D3D12ViewResource
{
	friend class D3D12DeviceResource;
public:
	D3D12ViewResource(class D3D12DeviceResource* DeviceContext, Window* window, bool bNeedGUI);
	~D3D12ViewResource();
private:
	void ClearRenderTargetView(ID3D12GraphicsCommandList* CommandList, float color[4]);
	void ClearDepthStencilView(ID3D12GraphicsCommandList* CommandList);
	ID3D12Resource* GetRenderTarget(uint32 Index)const;
	uint32 GetCurrentBackBufferIndex()const;
	void Resize();
	void Reset();
	HRESULT Present();
private:
	ComPtr<IDXGISwapChain3>			SwapChain;
	ComPtr<ID3D12Resource>			RenderTargets[MAX_BACK_BUFFER_COUNT];
	ComPtr<ID3D12Resource>			DepthStencilBuffer;
private:
	uint32							BackBufferIndex;
	ComPtr<ID3D12DescriptorHeap>	RTVHeap;
	ComPtr<ID3D12DescriptorHeap>	DSVHeap;
	ComPtr<ID3D12DescriptorHeap>	SRVHeap_GUI;
	class D3D12DeviceResource*		pDeviceContext;
	Window*							pWindow;
};