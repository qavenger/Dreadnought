#include <stdafx.h>
#include "D3D12Device.h"
#include "D3D12Texture.h"
#include "D3D12IndexBuffer.h"
#include "D3D12VertexBuffer.h"
#include "D3D12Shader.h"
#include "D3D12PipelineStateObject.h"
#include "D3D12RenderTarget.h"
#include "D3D12ConstantBuffer.h"

void D3D12Device::Init()
{
	//Create d3d12 factory
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&Factory)));

	//Init Dxgi Adapter
	InitDXGIAdapter();
	 
	//Create d3d12 device
	ThrowIfFailed(D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&Device)));

	//Create fence
	CurrentFence = 0;
	ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

	//Get descriptor size
	RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	CbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//Create command queue
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(Device->CreateCommandQueue(
		&QueueDesc,
		IID_PPV_ARGS(&CommandQueue)));

	//Create command allocator
	ThrowIfFailed(Device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&CommandAllocator)));

	//Create command list
	ThrowIfFailed(Device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		CommandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&CommandList)));
	//Close CommandList
	CommandList->Close();

	InitPlatformDenpendentMap();
}

void D3D12Device::Destroy()
{
	
}

void D3D12Device::CreateSwapChain(
	HWND Handle,
	uint32 Width,
	uint32 Height,
	uint32 BufferCount,
	ETextureFormat BBFormat,
	ETextureFormat DSFormat)
{
	IRHIDevice::CreateSwapChain(Handle, Width, Height, BufferCount, BBFormat, DSFormat);

	TextureDesc Desc;
	Desc.Dimension = ETextureDimension::TD_Texture2D;
	Desc.Format = BackBufferFormat;
	Desc.MipmapLevels = 1;
	Desc.TextureWidth = WindowWidth;
	Desc.TextureHeight = WindowHeight;
	Desc.TextureDepth = 1;
	Desc.Name = L"BackBuffer";
	Desc.UseForDepth = false;
	BackBuffer.resize(BackBufferCount);
	for (uint32 Index = 0; Index < BackBufferCount; ++Index)
	{
		D3D12Texture* Tex = (D3D12Texture*)CreateTexture();
		Tex->SetDesc(Desc);
		BackBuffer[Index].SetTexture(Tex);
	}

	SwapChain.Reset();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferDesc.Width = WindowWidth;
	SwapChainDesc.BufferDesc.Height = WindowHeight;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferDesc.Format = TextureFormatMap[(uint32)BackBufferFormat];//BackBufferFormat;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = BackBufferCount;
	SwapChainDesc.OutputWindow = WindowHandle;
	SwapChainDesc.Windowed = true;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(Factory->CreateSwapChain(
		CommandQueue.Get(),
		&SwapChainDesc,
		&SwapChain));

	//Create RTV Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc;
	RtvHeapDesc.NumDescriptors = BackBufferCount;
	RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	RtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(Device->CreateDescriptorHeap(
		&RtvHeapDesc, IID_PPV_ARGS(&RtvHeap)));

	//Create DSV Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC DsvHeapDesc;
	DsvHeapDesc.NumDescriptors = 1;
	DsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	DsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(Device->CreateDescriptorHeap(
		&DsvHeapDesc,
		IID_PPV_ARGS(&DsvHeap)));

	//Create CBV/SRV/UAV Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC CbvSrvUavDesc;
	CbvSrvUavDesc.NumDescriptors = 1;
	CbvSrvUavDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	CbvSrvUavDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	CbvSrvUavDesc.NodeMask = 0;
	ThrowIfFailed(Device->CreateDescriptorHeap(
		&CbvSrvUavDesc,
		IID_PPV_ARGS(&CbvSrvUavHeap)));
}

void D3D12Device::Resize(
	uint32 Width,
	uint32 Height)
{
	WaitForGPU();
	ResetCommandList();

	IRHIDevice::Resize(Width, Height);

	ThrowIfFailed(SwapChain->ResizeBuffers(
		BackBufferCount,
		WindowWidth, WindowHeight,
		TextureFormatMap[(uint32)BackBufferFormat],
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	//create backbuffer
	for (uint32 Index = 0; Index < BackBufferCount; ++Index)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHeapHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart(), Index, RtvDescriptorSize);
		D3D12Texture* Tex = (D3D12Texture*)BackBuffer[Index].GetTexture();
		ThrowIfFailed(SwapChain->GetBuffer(Index, IID_PPV_ARGS(&Tex->GetResource())));
		ThrowIfFailed(Tex->GetResource()->SetName(L"BackBuffer"));
		Device->CreateRenderTargetView(Tex->GetResource().Get(), nullptr, RtvHeapHandle);
		BackBuffer[Index].SetRenderTargetView(RtvHeapHandle);
	}

	//create depth stencil buffer
	TextureDesc DepthStencilDesc;
	DepthStencilDesc.Dimension = ETextureDimension::TD_Texture2D;
	DepthStencilDesc.Format = DepthStencilFormat;
	DepthStencilDesc.TextureWidth = WindowWidth;
	DepthStencilDesc.TextureHeight = WindowHeight;
	DepthStencilDesc.TextureDepth = 1;
	DepthStencilDesc.MipmapLevels = 1;
	DepthStencilDesc.Name = L"SceneDepthZ";
	DepthStencilDesc.UseForDepth = true;
	D3D12Texture* DepthTexture = (D3D12Texture*)CreateTexture();
	DepthTexture->SetDesc(DepthStencilDesc);
	BuildTexture(DepthTexture);
	DepthStencilBuffer.SetTexture(DepthTexture);
	DepthStencilBuffer.SetRenderTargetView(GetDepthStencilView());

	Device->CreateDepthStencilView(
		DepthTexture->GetResource().Get(),
		nullptr,
		GetDepthStencilView());
	
	Transition(DepthTexture->GetResource().Get(), EResourceState::RS_Common, EResourceState::RS_DepthWrite);

	FlushCommandQueueSync();
}

void D3D12Device::InitDXGIAdapter()
{
	bool bDebugDXGI = false;
#if defined(_DEBUG) || defined(DEBUG)
	{
		ComPtr<ID3D12Debug> DebugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)));
		DebugController->EnableDebugLayer();

		ComPtr<IDXGIInfoQueue> DxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DxgiInfoQueue))))
		{
			bDebugDXGI = true;

			DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
	}
#endif

	QueryAdapters();
}

void D3D12Device::QueryAdapters()
{
	IDXGIAdapter* Adapter;
	ComPtr<IDXGIFactory6> factory6;
	uint32 Index = 0;
	while (Factory->EnumAdapters(Index, &Adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC AdapterDesc;
		Adapter->GetDesc(&AdapterDesc);

		std::wstring Text = L"***Adapter: ";
		Text += AdapterDesc.Description;
		Text += L"\n";
	//	OutputDebugString(Text.c_str());

		Adapters.push_back(Adapter);
		++Index;
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Device::GetCurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		RtvHeap->GetCPUDescriptorHandleForHeapStart(), 
		CurrentBackBufferIndex, 
		RtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Device::GetDepthStencilView() const
{
	return DsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3D12Device::InitPlatformDenpendentMap()
{
	TextureFormatMap[(uint32)ETextureFormat::TF_Unknown]              = DXGI_FORMAT_UNKNOWN;
	TextureFormatMap[(uint32)ETextureFormat::TF_R16]                  = DXGI_FORMAT_R16_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R16G16]               = DXGI_FORMAT_R16G16_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R16G16B16A16]         = DXGI_FORMAT_R16G16B16A16_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R32]                  = DXGI_FORMAT_R32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R32G32]               = DXGI_FORMAT_R32G32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R32G32B32]            = DXGI_FORMAT_R32G32B32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R32G32B32A32]         = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R8G8B8A8]             = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureFormatMap[(uint32)ETextureFormat::TF_D24S8]                = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureFormatMap[(uint32)ETextureFormat::TF_D32]                  = DXGI_FORMAT_D32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_D32S8]                = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	CullModeMap[(uint32)ECullMode::CM_None]                           = D3D12_CULL_MODE_NONE;
	CullModeMap[(uint32)ECullMode::CM_Back]                           = D3D12_CULL_MODE_BACK;
	CullModeMap[(uint32)ECullMode::CM_Front]                          = D3D12_CULL_MODE_FRONT;

	FillModeMap[(uint32)EFillMode::FM_Wireframe]                      = D3D12_FILL_MODE_WIREFRAME;
	FillModeMap[(uint32)EFillMode::FM_Solid]                          = D3D12_FILL_MODE_SOLID;

	PrimitiveTopologyTypeMap[(uint32)EPrimitiveTopology::PT_Point]    = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	PrimitiveTopologyTypeMap[(uint32)EPrimitiveTopology::PT_Line]     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	PrimitiveTopologyTypeMap[(uint32)EPrimitiveTopology::PT_Triangle] = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	PrimitiveTopologyMap[(uint32)EPrimitiveTopology::PT_Point]        = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	PrimitiveTopologyMap[(uint32)EPrimitiveTopology::PT_Line]         = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	PrimitiveTopologyMap[(uint32)EPrimitiveTopology::PT_Triangle]     = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	BlendModeMap[(uint32)EBlendMode::BM_One]                          = D3D12_BLEND_ONE;
	BlendModeMap[(uint32)EBlendMode::BM_Zero]                         = D3D12_BLEND_ZERO;
	BlendModeMap[(uint32)EBlendMode::BM_SrcColor]                     = D3D12_BLEND_SRC_COLOR;
	BlendModeMap[(uint32)EBlendMode::BM_InverseSrcColor]              = D3D12_BLEND_INV_SRC_COLOR;
	BlendModeMap[(uint32)EBlendMode::BM_SrcAlpha]                     = D3D12_BLEND_SRC_ALPHA;
	BlendModeMap[(uint32)EBlendMode::BM_InverseSrcAlpha]              = D3D12_BLEND_INV_SRC_ALPHA;
	BlendModeMap[(uint32)EBlendMode::BM_DestColor]                    = D3D12_BLEND_DEST_COLOR;
	BlendModeMap[(uint32)EBlendMode::BM_InverseDestColor]             = D3D12_BLEND_INV_DEST_COLOR;
	BlendModeMap[(uint32)EBlendMode::BM_DestAlpha]                    = D3D12_BLEND_DEST_ALPHA;
	BlendModeMap[(uint32)EBlendMode::BM_InverseDestAlpha]             = D3D12_BLEND_INV_DEST_ALPHA;

	BlendOperatorMap[(uint32)EBlendOperator::BO_Add]                  = D3D12_BLEND_OP_ADD;
	BlendOperatorMap[(uint32)EBlendOperator::BO_Sub]                  = D3D12_BLEND_OP_SUBTRACT;
	BlendOperatorMap[(uint32)EBlendOperator::BO_ReverseSub]           = D3D12_BLEND_OP_REV_SUBTRACT;
	BlendOperatorMap[(uint32)EBlendOperator::BO_Min]                  = D3D12_BLEND_OP_MIN;
	BlendOperatorMap[(uint32)EBlendOperator::BO_Max]                  = D3D12_BLEND_OP_MAX;

	ShaderTypeMap[(uint32)EShaderType::ST_Vertex]                     = "vs_5_0";
	ShaderTypeMap[(uint32)EShaderType::ST_Pixel]                      = "ps_5_0";
	ShaderTypeMap[(uint32)EShaderType::ST_Geometry]                   = "gs_5_0";
	ShaderTypeMap[(uint32)EShaderType::ST_Computer]                   = "cs_5_0";

	WriteMaskMap[(uint32)EWriteMask::WM_R]                            = D3D12_COLOR_WRITE_ENABLE_RED;
	WriteMaskMap[(uint32)EWriteMask::WM_G]                            = D3D12_COLOR_WRITE_ENABLE_GREEN;
	WriteMaskMap[(uint32)EWriteMask::WM_B]                            = D3D12_COLOR_WRITE_ENABLE_BLUE;
	WriteMaskMap[(uint32)EWriteMask::WM_A]                            = D3D12_COLOR_WRITE_ENABLE_ALPHA;
	WriteMaskMap[(uint32)EWriteMask::WM_RGB]                          = (D3D12_COLOR_WRITE_ENABLE)(D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE);
	WriteMaskMap[(uint32)EWriteMask::WM_RGBA]                         = D3D12_COLOR_WRITE_ENABLE_ALL;
 
	DepthFunctionMap[(uint32)EDepthFunction::DF_Never]                = D3D12_COMPARISON_FUNC_NEVER;
	DepthFunctionMap[(uint32)EDepthFunction::DF_Less]                 = D3D12_COMPARISON_FUNC_LESS;
	DepthFunctionMap[(uint32)EDepthFunction::DF_Equal]                = D3D12_COMPARISON_FUNC_EQUAL;
	DepthFunctionMap[(uint32)EDepthFunction::DF_LessEqual]            = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	DepthFunctionMap[(uint32)EDepthFunction::DF_Greater]              = D3D12_COMPARISON_FUNC_GREATER;
	DepthFunctionMap[(uint32)EDepthFunction::DF_NotEqual]             = D3D12_COMPARISON_FUNC_NOT_EQUAL;
	DepthFunctionMap[(uint32)EDepthFunction::DF_GreaterEqual]         = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	DepthFunctionMap[(uint32)EDepthFunction::DF_Always]               = D3D12_COMPARISON_FUNC_ALWAYS;

	TextureFilterMap[(uint32)ETextureFilter::TF_Point]                = D3D12_FILTER_MIN_MAG_MIP_POINT;
	TextureFilterMap[(uint32)ETextureFilter::TF_Linear]               = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	TextureFilterMap[(uint32)ETextureFilter::TF_Trilinear]            = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	TextureFilterMap[(uint32)ETextureFilter::TF_Anisotropic]          = D3D12_FILTER_ANISOTROPIC;

	TextureWrapModeMap[(uint32)ETextureWrapMode::TWM_Wrap]            = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	TextureWrapModeMap[(uint32)ETextureWrapMode::TWM_Mirror]          = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	TextureWrapModeMap[(uint32)ETextureWrapMode::TWM_Clamp]           = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	ResourceStateMap[(uint32)EResourceState::RS_Common]               = D3D12_RESOURCE_STATE_COMMON;
	ResourceStateMap[(uint32)EResourceState::RS_Read]                 = D3D12_RESOURCE_STATE_GENERIC_READ;
	ResourceStateMap[(uint32)EResourceState::RS_RenderTarget]         = D3D12_RESOURCE_STATE_RENDER_TARGET;
	ResourceStateMap[(uint32)EResourceState::RS_CopyDest]             = D3D12_RESOURCE_STATE_COPY_DEST;
	ResourceStateMap[(uint32)EResourceState::RS_DepthWrite]           = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	ResourceStateMap[(uint32)EResourceState::RS_Present]              = D3D12_RESOURCE_STATE_PRESENT;
}  

RHITexture* D3D12Device::CreateTexture()
{
	return new D3D12Texture();
}

RHIIndexBuffer* D3D12Device::CreateIndexBuffer()
{
	return new D3D12IndexBuffer();
}

RHIVertexBuffer* D3D12Device::CreateVertexBuffer()
{
	return new D3D12VertexBuffer();
}

RHIShader* D3D12Device::CreateShader()
{

	return new D3D12Shader();
}

RHIPipelineStateObject* D3D12Device::CreatePipelineStateObject()
{
	return new D3D12PipelineStateObject();
}

RHIConstantBuffer* D3D12Device::CreateConstantBuffer()
{
	return new D3D12ConstantBuffer();
}

void D3D12Device::SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth) const
{
	D3D12_VIEWPORT VP;
	VP.TopLeftX = X;
	VP.TopLeftY = Y;
	VP.Width = Width;
	VP.Height = Height;
	VP.MinDepth = MinDepth;
	VP.MaxDepth = MaxDepth;

	CommandList->RSSetViewports(1, &VP);
}

void D3D12Device::SetScissor(float Left, float Top, float Right, float Bottom) const
{
	tagRECT ScissorRect = { (int32)Left, (int32)Top, (int32)Right, (int32)Bottom };
	CommandList->RSSetScissorRects(1, &ScissorRect);
}

void D3D12Device::BeginRenderPass(const RHIRenderPassInfo& RPInfo)
{
	D3D12_CPU_DESCRIPTOR_HANDLE SceneColorRenderTargetView[MAX_RENDER_TARGET];
	for (uint32 Index = 0; Index < RPInfo.NumRenderTarget; ++Index)
	{
		D3D12RenderTarget* SceneColorRenderTarget = (D3D12RenderTarget*)RPInfo.SceneColor[Index];
		SceneColorRenderTargetView[Index] = SceneColorRenderTarget->GetRenderTargetView();

		const ClearCDSValue& ClearValue = RPInfo.ClearColor[Index];
		if (ClearValue.IsClearColorValid())
		{
			float ClearColor[] = { ClearValue.ClearColorR, ClearValue.ClearColorG, ClearValue.ClearColorB, ClearValue.ClearColorA };
			CommandList->ClearRenderTargetView(SceneColorRenderTargetView[Index], ClearColor, 0, nullptr);
		}
	}

	D3D12_CLEAR_FLAGS DSClearFlag = (D3D12_CLEAR_FLAGS)0;
	if (RPInfo.IsClearDepthValid()) DSClearFlag |= D3D12_CLEAR_FLAG_DEPTH;
	if (RPInfo.IsClearStencilValid()) DSClearFlag |= D3D12_CLEAR_FLAG_STENCIL;
	D3D12RenderTarget* DepthRenderTarget = (D3D12RenderTarget*)RPInfo.SceneDepthZ;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthRenderTargetView = DepthRenderTarget->GetRenderTargetView();
	CommandList->ClearDepthStencilView(DepthRenderTargetView, DSClearFlag, RPInfo.ClearDepth, RPInfo.ClearStencil, 0, nullptr);

	CommandList->OMSetRenderTargets(RPInfo.NumRenderTarget, SceneColorRenderTargetView, true, &DepthRenderTargetView);
}

void D3D12Device::EndRenderPass()
{
	// some resolve operation

}

void D3D12Device::ResetCommandList()
{
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));
}

void D3D12Device::FlushCommandQueue() 
{
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* Lists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(ARRAYSIZE(Lists), Lists);
}

void D3D12Device::FlushCommandQueueSync()
{
	FlushCommandQueue();
	WaitForGPU();
}

void D3D12Device::WaitForGPU()
{
	CurrentFence++;

	ThrowIfFailed(CommandQueue->Signal(Fence.Get(), CurrentFence));
	if (Fence->GetCompletedValue() < CurrentFence)
	{
		HANDLE EventHandle = CreateEventEx(nullptr, " ", false, EVENT_ALL_ACCESS);

		ThrowIfFailed(Fence->SetEventOnCompletion(CurrentFence, EventHandle));
		WaitForSingleObject(EventHandle, INFINITE);
		CloseHandle(EventHandle);
	}
}

void D3D12Device::BeginScopeEvent(const char* EventName)
{
	PIXBeginEvent(CommandList.Get(), 0, EventName);
}

void D3D12Device::EndScopeEvent()
{
	PIXEndEvent(CommandList.Get());
}

void D3D12Device::Present()
{
	SwapChain->Present(0, 0);
	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % 2;
}

void D3D12Device::Transition(void* Resource, EResourceState Before, EResourceState After) const
{
	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition((ID3D12Resource*)Resource,
		ResourceStateMap[(uint32)Before], ResourceStateMap[(uint32)After]);
	CommandList->ResourceBarrier(1, &Barrier);
}

void D3D12Device::BeginFrame()
{
	ThrowIfFailed(CommandAllocator->Reset());
	ResetCommandList();

	Transition(GetRHIBackbufferRenderTarget()->GetTexture()->GetResourceRaw(), EResourceState::RS_Present, EResourceState::RS_RenderTarget);
}

void D3D12Device::EndFrame()
{
	{
		SCOPE_EVENT(RenderFinish)
		Transition(GetRHIBackbufferRenderTarget()->GetTexture()->GetResourceRaw(), EResourceState::RS_RenderTarget, EResourceState::RS_Present);
	}

	FlushCommandQueue();
	Present();
	WaitForGPU();
}

void D3D12Device::BuildTexture(RHITexture* Tex)
{
	TextureDesc Desc = Tex->GetDesc();
	D3D12Texture* D3DTexture = (D3D12Texture*)Tex;

	D3D12_RESOURCE_DESC ResourceDesc = {};
	DXGI_SAMPLE_DESC SampleDesc;
	switch (Desc.Dimension)
	{
	case ETextureDimension::TD_TextureBuffer:
		SampleDesc.Count = 1;
		SampleDesc.Quality = 0;
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Alignment = 0;
		ResourceDesc.Width = Desc.TextureWidth;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = TextureFormatMap[(uint32)Desc.Format];
		ResourceDesc.SampleDesc = SampleDesc;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		break;
	case ETextureDimension::TD_Texture1D:

		break;
	case ETextureDimension::TD_Texture2D:
		SampleDesc.Count = 1;
		SampleDesc.Quality = 0;
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.Width = Desc.TextureWidth;
		ResourceDesc.Height = Desc.TextureHeight;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = Desc.MipmapLevels;
		ResourceDesc.Format = TextureFormatMap[(uint32)Desc.Format];
		ResourceDesc.SampleDesc = SampleDesc;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		if (Desc.UseForDepth) ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		else ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		break;
	case ETextureDimension::TD_Texture3D:

		break;
	case ETextureDimension::TD_TextureCube:

		break;
	default:
		// throw unknow type error
		break;

	}

	//set properties
	D3D12_HEAP_PROPERTIES Properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.Format = TextureFormatMap[(uint32)Desc.Format];
	ClearValue.DepthStencil.Depth = 1.f;
	ClearValue.DepthStencil.Stencil = 0;

	//create resource
	ThrowIfFailed(Device->CreateCommittedResource(
		&Properties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		Desc.UseForDepth ? &ClearValue : nullptr, // need not to clear
		IID_PPV_ARGS(&D3DTexture->GetResource())));

	D3DTexture->GetResource()->SetName(Desc.Name.c_str());
}

ComPtr<ID3D12RootSignature> D3D12Device::CreateRootSignature(ComPtr<ID3DBlob>& Blob)
{
	ComPtr<ID3D12RootSignature> RootSignature;

	ThrowIfFailed(Device->CreateRootSignature(
		0,
		Blob->GetBufferPointer(),
		Blob->GetBufferSize(),
		IID_PPV_ARGS(&RootSignature)));

	return RootSignature;
}

ComPtr<ID3D12Resource> D3D12Device::CreateDefaultBuffer(
	const void* Data,
	uint32 SizeInBytes,
	ComPtr<ID3D12Resource>& UploadBuffer)
{
	ComPtr<ID3D12Resource> DefaultBuffer;
	CD3DX12_HEAP_PROPERTIES DefaultProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC DefaultDesc = CD3DX12_RESOURCE_DESC::Buffer(SizeInBytes);
	ThrowIfFailed(Device->CreateCommittedResource(
		&DefaultProperties,
		D3D12_HEAP_FLAG_NONE,
		&DefaultDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&DefaultBuffer)));

	CD3DX12_HEAP_PROPERTIES UploadProperties(D3D12_HEAP_TYPE_UPLOAD);
	ThrowIfFailed(Device->CreateCommittedResource(
		&UploadProperties,
		D3D12_HEAP_FLAG_NONE,
		&DefaultDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&UploadBuffer)));

	D3D12_SUBRESOURCE_DATA SubResourceData = {};
	SubResourceData.pData = Data;
	SubResourceData.RowPitch = SizeInBytes;
	SubResourceData.SlicePitch = SubResourceData.RowPitch;

	Transition(DefaultBuffer.Get(), EResourceState::RS_Common, EResourceState::RS_CopyDest);
	UpdateSubresources<1>(CommandList.Get(), DefaultBuffer.Get(), UploadBuffer.Get(), 0, 0, 1, &SubResourceData);
	Transition(DefaultBuffer.Get(), EResourceState::RS_CopyDest, EResourceState::RS_Read);

	return DefaultBuffer;
}

void D3D12Device::BuildIndexBuffer(RHIIndexBuffer* IndexBuffer)
{
	D3D12IndexBuffer* D3DIB = (D3D12IndexBuffer*)IndexBuffer;
	D3DIB->IndexBufferGPU = CreateDefaultBuffer(D3DIB->GetData(), D3DIB->GetDataSize(), D3DIB->UploadBuffer);
}

void D3D12Device::BuildVertexBuffer(RHIVertexBuffer* VertexBuffer)
{
	D3D12VertexBuffer* D3DVB = (D3D12VertexBuffer*)VertexBuffer;
	D3DVB->VertexBufferGPU = CreateDefaultBuffer(D3DVB->GetData(), D3DVB->GetDataSize(), D3DVB->UploadBuffer);
}

void D3D12Device::BuildShader(RHIShader* Shader)
{
	D3D12Shader* D3DShader = (D3D12Shader*)Shader;
	std::wstring ShaderFile = D3DShader->GetShaderFile();
	std::string ShaderEntry = D3DShader->GetShaderEntry();
	std::string ShaderTarget = ShaderTypeMap[(uint32)D3DShader->GetShaderType()];
	uint32 CompileFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	CompileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ComPtr<ID3DBlob> Errors;
	HRESULT Result = S_OK;
	Result = D3DCompileFromFile(ShaderFile.c_str(), nullptr, nullptr, ShaderEntry.c_str(), ShaderTarget.c_str(), CompileFlag, 0, &D3DShader->GetShaderCode(), &Errors);
	if (Errors)
	{
		OutputDebugString((char*)Errors->GetBufferPointer());
	}

	ThrowIfFailed(Result);
}

void D3D12Device::BuildPipelineStateObject(RHIPipelineStateObject* PSO)
{
	D3D12PipelineStateObject* D3DPSO = (D3D12PipelineStateObject*)PSO;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
	ZeroMemory(&PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	D3D12Shader* VertexShader = (D3D12Shader*)D3DPSO->VertexShader;
	D3D12Shader* PixelShader = (D3D12Shader*)D3DPSO->PixelShader;
	D3D12Shader* GeometryShader = (D3D12Shader*)D3DPSO->GeometryShader;
	PSODesc.InputLayout.pInputElementDescs = D3DPSO->GetInputLayout();
	PSODesc.InputLayout.NumElements = D3DPSO->GetInputLayoutSize();
	PSODesc.pRootSignature = D3DPSO->GetRootSignature().Get();
	PSODesc.VS.pShaderBytecode = VertexShader->GetShaderCode()->GetBufferPointer();
	PSODesc.VS.BytecodeLength = VertexShader->GetShaderCode()->GetBufferSize();
	if (PixelShader)
	{
		PSODesc.PS.pShaderBytecode = PixelShader->GetShaderCode()->GetBufferPointer();
		PSODesc.PS.BytecodeLength = PixelShader->GetShaderCode()->GetBufferSize();
	}
	if (GeometryShader)
	{
		PSODesc.GS.pShaderBytecode = GeometryShader->GetShaderCode()->GetBufferPointer();
		PSODesc.GS.BytecodeLength = GeometryShader->GetShaderCode()->GetBufferSize();
	}
	PSODesc.RasterizerState.CullMode = CullModeMap[(uint32)D3DPSO->CullMode];
	PSODesc.RasterizerState.FillMode = FillModeMap[(uint32)D3DPSO->FillMode];
	PSODesc.RasterizerState.FrontCounterClockwise = false;
	PSODesc.RasterizerState.DepthBias = 0;
	PSODesc.RasterizerState.DepthBiasClamp = 0.f;
	PSODesc.RasterizerState.SlopeScaledDepthBias = 0.f;
	PSODesc.RasterizerState.DepthClipEnable = false;
	PSODesc.RasterizerState.MultisampleEnable = false;
	PSODesc.RasterizerState.AntialiasedLineEnable = false;
	PSODesc.RasterizerState.ForcedSampleCount = 0;
	PSODesc.BlendState.AlphaToCoverageEnable = false;
	PSODesc.BlendState.IndependentBlendEnable = false;
	for (uint32 Index = 0; Index < D3DPSO->NumRenderTarget; ++Index)
	{
		bool BlendEnable                       = D3DPSO->State.IsRenderTargetEnableBlend(Index);
		D3D12_BLEND ColorSrc                   = BlendModeMap[(uint32)D3DPSO->State.ColorSrcBlendMode[Index]];
		D3D12_BLEND ColorDest                  = BlendModeMap[(uint32)D3DPSO->State.ColorDestBlendMode[Index]];
		D3D12_BLEND_OP ColorOP                 = BlendOperatorMap[(uint32)D3DPSO->State.ColorBlendOperator[Index]];
		D3D12_BLEND AlphaSrc                   = BlendModeMap[(uint32)D3DPSO->State.AlphaSrcBlendMode[Index]];
		D3D12_BLEND AlphaDest                  = BlendModeMap[(uint32)D3DPSO->State.AlphaDestBlendMode[Index]];
		D3D12_BLEND_OP AlphaOP                 = BlendOperatorMap[(uint32)D3DPSO->State.AlphaBlendOperator[Index]];
		D3D12_COLOR_WRITE_ENABLE WriteMask     = WriteMaskMap[(uint32)D3DPSO->State.WriteMask[Index]];
		const D3D12_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc =
		{
			BlendEnable,FALSE,
			ColorSrc, ColorDest, ColorOP,
			AlphaSrc, AlphaDest, AlphaOP,
			D3D12_LOGIC_OP_NOOP,
			(uint8)WriteMask,
		};
		PSODesc.BlendState.RenderTarget[Index] = RenderTargetBlendDesc;
	}
	PSODesc.DepthStencilState.DepthEnable = D3DPSO->EnableDepthTest;
	PSODesc.DepthStencilState.DepthWriteMask = D3DPSO->EnableDepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
	PSODesc.DepthStencilState.DepthFunc = DepthFunctionMap[(uint32)D3DPSO->DepthFunction];
	PSODesc.DepthStencilState.StencilEnable = D3DPSO->EnableStencilTest;
	PSODesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	PSODesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	PSODesc.DepthStencilState.FrontFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	PSODesc.DepthStencilState.BackFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	PSODesc.SampleMask = UINT_MAX;
	PSODesc.PrimitiveTopologyType = PrimitiveTopologyTypeMap[(uint32)D3DPSO->GsHsPrimitiveTopology];
	if (D3DPSO->IsBackbuffer)
	{
		PSODesc.RTVFormats[0] = TextureFormatMap[(uint32)BackBufferFormat];
		PSODesc.DSVFormat = TextureFormatMap[(uint32)DepthStencilFormat];
		PSODesc.NumRenderTargets = 1;
	}
	else
	{
		for (uint32 Index = 0; Index < D3DPSO->NumRenderTarget; ++Index)
			PSODesc.RTVFormats[Index] = TextureFormatMap[(uint32)D3DPSO->SceneColorRenderTarget[Index]->GetTexture()->GetDesc().Format];
		PSODesc.DSVFormat = TextureFormatMap[(uint32)D3DPSO->DepthStencilRenderTarget->GetTexture()->GetDesc().Format];
		PSODesc.NumRenderTargets = D3DPSO->NumRenderTarget;
	}
	PSODesc.SampleDesc.Count = 1;
	PSODesc.SampleDesc.Quality = 0;

	ThrowIfFailed(Device->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&D3DPSO->GetPSO())));
}

void D3D12Device::BuildConstantBuffer(RHIConstantBuffer* ConstantBuffer)
{
	D3D12ConstantBuffer* D3DConstantBuffer = (D3D12ConstantBuffer*)ConstantBuffer;
	uint32 ElementSize = D3DConstantBuffer->GetElementSize();
	uint32 ElementCount = D3DConstantBuffer->GetElementCount();
	uint32 Size = GMath::RoundUp256(ElementSize * ElementCount);

	CD3DX12_HEAP_PROPERTIES Properties(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(Size);
	ThrowIfFailed(Device->CreateCommittedResource(
		&Properties,
		D3D12_HEAP_FLAG_NONE,
		&BufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&D3DConstantBuffer->GetResource())));
}

void D3D12Device::SetPipelineStateObject(RHIPipelineStateObject* PSO)
{
	D3D12PipelineStateObject* D3DPSO = (D3D12PipelineStateObject*)PSO;
	CommandList->SetPipelineState(D3DPSO->GetPSO().Get());
}

void D3D12Device::DrawElements(const DrawInfo& Info)
{
	ThrowIfFalse(Info.VertexBuffer != nullptr, "VertexBuffer Is None");

	D3D12PipelineStateObject* D3DPSO = (D3D12PipelineStateObject*)Info.PSO;
	SetPipelineStateObject(Info.PSO);

	CommandList->SetGraphicsRootSignature(D3DPSO->GetRootSignature().Get());
	/*ID3D12DescriptorHeap* DescriptorHeap[] = { CbvSrvUavHeap.Get() };
	CommandList->SetDescriptorHeaps(_countof(DescriptorHeap), DescriptorHeap);
	CommandList->SetGraphicsRootDescriptorTable(1, CbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());*/
	ThrowIfFalse(D3DPSO->ConstantBuffers.size() == D3DPSO->GetShaderConstantBuffersCount(), "Input size of constant buffer must be same as the size of shader constant buffer");
	for (uint32 Index = 0; Index < Info.PSO->ConstantBuffers.size(); ++Index)
	{
		D3D12ConstantBuffer* D3DCB = (D3D12ConstantBuffer*)D3DPSO->ConstantBuffers[Index];
		D3D12_GPU_VIRTUAL_ADDRESS Address = D3DCB->GetResource()->GetGPUVirtualAddress();
		CommandList->SetGraphicsRootConstantBufferView(Index, Address);
	}

	D3D12VertexBuffer* D3DVB = (D3D12VertexBuffer*)Info.VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = D3DVB->GetVertexBufferView();
	CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	CommandList->IASetPrimitiveTopology(PrimitiveTopologyMap[(uint32)D3DPSO->PrimitiveTopology]);

	bool IndexExists = Info.IndexBuffer != nullptr;
	if (IndexExists)
	{
		D3D12IndexBuffer* D3DIB = (D3D12IndexBuffer*)Info.IndexBuffer;
		D3D12_INDEX_BUFFER_VIEW IndexBufferView = D3DIB->GetIndexBufferView();
		CommandList->IASetIndexBuffer(&IndexBufferView);
		CommandList->DrawIndexedInstanced(D3DIB->GetIndexCount(), 1, 0, 0, 0);
	}
	else
	{
		CommandList->DrawInstanced(D3DVB->GetVertexCount(), 1, 0, 0);
	}
}