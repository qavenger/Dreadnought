#pragma once

#include "../RHIDevice.h"

class D3D12Device : public IRHIDevice
{
public:
	virtual void Init();
	virtual void Destroy();

	virtual void Resize(
		uint32 Width,
		uint32 Height);

	virtual void CreateSwapChain(
		HWND Handle,
		uint32 Width,
		uint32 Height,
		uint32 BufferCount,
		ETextureFormat BBFormat,
		ETextureFormat DSFormat);

	virtual void BeginFrame();
	virtual void EndFrame();
	virtual void BeginRenderPass(RHIRenderPassInfo& RPInfo);
	virtual void EndRenderPass();
	virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) const;
	virtual void SetScissor(float Left, float Top, float Right, float Bottom) const;
	virtual void FlushCommandQueue();
	virtual void FlushCommandQueueSync();
	virtual void WaitForGPU();
	virtual void Present() const;

	//Create Resource Function
	virtual void CreateTextureXD(TextureDesc& Desc, ITexture* Tex);
	virtual IIndexBuffer* CreateIndexBuffer();
	virtual IVertexBuffer* CreateVertexBuffer();
	virtual IShader* CreateShader();


	virtual void SetIndexBuffer(IIndexBuffer* IndexBuffer);
	virtual void SetVertexBuffer(IVertexBuffer* VertexBuffer);
	virtual void SetShader(IShader* Shader);
	virtual void SetPipelineStateObject(IPipelineStateObject* PSO);

	virtual void DrawIndexedInstanced(IIndexBuffer* IndexBuffer, IVertexBuffer* VertexBuffer);

private:
	void InitDXGIAdapter();
	void QueryAdapters();
	ComPtr<ID3D12Resource> CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	void InitPlatformDenpendentMap();

	ComPtr<ID3D12Resource> CreateDefaultBuffer(
		const void* Data,
		uint32 SizeInBytes,
		ComPtr<ID3D12Resource>& UploadBuffer);

private:
	ComPtr<IDXGIFactory>						      Factory;
	std::vector<ComPtr<IDXGIAdapter>>			      Adapters;
	ComPtr<ID3D12Device>                              Device;
	ComPtr<ID3D12GraphicsCommandList>			      CommandList;
	ComPtr<ID3D12CommandAllocator>                    CommandAllocator;
	ComPtr<ID3D12CommandQueue>			      		  CommandQueue;
	ComPtr<ID3D12Fence>                               Fence;
	uint64                                            CurrentFence;

	ComPtr<IDXGISwapChain>                            SwapChain;
	// special
	std::vector<ComPtr<ID3D12Resource>>               SwapChainBuffer;
	ComPtr<ID3D12Resource>                            DepthStencilBuffer;
	// special end
	ComPtr<ID3D12DescriptorHeap>                      RtvHeap;
	ComPtr<ID3D12DescriptorHeap>                      DsvHeap;
	uint32                                            RtvDescriptorSize;
	uint32										      DsvDescriptorSize;
	uint32										      CbvSrvUavDescriptorSize;

	std::unordered_map<ETextureFormat, DXGI_FORMAT>                            TextureFormatMap;
	std::unordered_map<ECullMode, D3D12_CULL_MODE>                             CullModeMap;
	std::unordered_map<EFillMode, D3D12_FILL_MODE>                             FillModeMap;
	std::unordered_map<EPrimitiveTopology, D3D12_PRIMITIVE_TOPOLOGY_TYPE>      PrimitiveTopologyMap;
	std::unordered_map<EBlendMode, D3D12_BLEND>                                BlendModeMap;
	std::unordered_map<EBlendOperator, D3D12_BLEND_OP>                         BlendOperatorMap;
	std::unordered_map<EWriteMask, D3D12_COLOR_WRITE_ENABLE>                   WriteMaskMap;
	std::unordered_map<EShaderType, std::string>                               ShaderTypeMap;
	std::unordered_map<EDepthFunction, D3D12_COMPARISON_FUNC>                  DepthFunctionMap;
};
