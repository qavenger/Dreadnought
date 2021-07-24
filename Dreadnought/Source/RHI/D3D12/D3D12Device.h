#pragma once

#include "../RHIDevice.h"
#include "D3D12RenderTarget.h"

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

	virtual void BeginScopeEvent(const char* EventName);
	virtual void EndScopeEvent();
	virtual void BeginFrame();
	virtual void EndFrame();
	virtual void BeginRenderPass(const RHIRenderPassInfo& RPInfo);
	virtual void EndRenderPass();
	virtual void SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) const;
	virtual void SetScissor(float Left, float Top, float Right, float Bottom) const;
	virtual void ResetCommandList();
	virtual void FlushCommandQueue();
	virtual void FlushCommandQueueSync();
	virtual void WaitForGPU();
	virtual void Present();
	virtual void Transition(void* Resource, EResourceState Before, EResourceState After) const;

	//Create Resource Function
	virtual RHITexture* CreateTexture();
	virtual RHIIndexBuffer* CreateIndexBuffer();
	virtual RHIVertexBuffer* CreateVertexBuffer();
	virtual RHIShader* CreateShader();
	virtual RHIPipelineStateObject* CreatePipelineStateObject();
	virtual RHIConstantBuffer* CreateConstantBuffer();

	virtual void BuildTexture(RHITexture* Texture);
	virtual void BuildIndexBuffer(RHIIndexBuffer* IndexBuffer);
	virtual void BuildVertexBuffer(RHIVertexBuffer* VertexBuffer);
	virtual void BuildShader(RHIShader* Shader);
	virtual void BuildPipelineStateObject(RHIPipelineStateObject* PSO);
	virtual void BuildConstantBuffer(RHIConstantBuffer* ConstantBuffer);


	virtual void SetPipelineStateObject(RHIPipelineStateObject* PSO);
	virtual void DrawElements(const DrawInfo& Info);

	virtual RHIRenderTarget* GetRHIDepthRenderTarget() const { return (RHIRenderTarget*)&DepthStencilBuffer; }
	virtual RHIRenderTarget* GetRHIBackbufferRenderTarget() const { return (RHIRenderTarget*)&BackBuffer[CurrentBackBufferIndex]; }

public:
	ComPtr<ID3D12RootSignature> CreateRootSignature(ComPtr<ID3DBlob>& Blob);

private:
	void InitDXGIAdapter();
	void QueryAdapters();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
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
	std::vector<D3D12RenderTarget>                    BackBuffer;
	D3D12RenderTarget                                 DepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap>                      RtvHeap;
	ComPtr<ID3D12DescriptorHeap>                      DsvHeap;
	ComPtr<ID3D12DescriptorHeap>                      CbvSrvUavHeap;
	uint32                                            RtvDescriptorSize;
	uint32										      DsvDescriptorSize;
	uint32										      CbvSrvUavDescriptorSize;

	DXGI_FORMAT                                       TextureFormatMap[(uint32)ETextureFormat::TF_Num];
	D3D12_CULL_MODE                                   CullModeMap[(uint32)ECullMode::CM_Num];
	D3D12_FILL_MODE                                   FillModeMap[(uint32)EFillMode::FM_Num];
	// Specifies how the pipeline interprets geometry or hull shader input primitives
	D3D12_PRIMITIVE_TOPOLOGY_TYPE                     PrimitiveTopologyTypeMap[(uint32)EPrimitiveTopology::PT_Num]; 
	D3D12_PRIMITIVE_TOPOLOGY                          PrimitiveTopologyMap[(uint32)EPrimitiveTopology::PT_Num];
	D3D12_BLEND                                       BlendModeMap[(uint32)EBlendMode::BM_Num];
	D3D12_BLEND_OP                                    BlendOperatorMap[(uint32)EBlendOperator::BO_Num];
	D3D12_COLOR_WRITE_ENABLE                          WriteMaskMap[(uint32)EWriteMask::WM_Num];
	std::string                                       ShaderTypeMap[(uint32)EShaderType::ST_Num];
	D3D12_COMPARISON_FUNC                             DepthFunctionMap[(uint32)EDepthFunction::DF_Num];
	D3D12_FILTER                                      TextureFilterMap[(uint32)ETextureFilter::TF_Num];
	D3D12_TEXTURE_ADDRESS_MODE                        TextureWrapModeMap[(uint32)ETextureWrapMode::TWM_Num];
	D3D12_RESOURCE_STATES                             ResourceStateMap[(uint32)EResourceState::RS_Num];
};
