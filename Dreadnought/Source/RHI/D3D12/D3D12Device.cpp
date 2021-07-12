#include <stdafx.h>
#include "D3D12Device.h"
#include "D3D12Texture.h"
#include "D3D12IndexBuffer.h"
#include "D3D12VertexBuffer.h"
#include "D3D12Shader.h"
#include "D3D12PipelineStateObject.h"
#include "D3D12RenderTarget.h"

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
	SwapChainDesc.BufferCount = SwapChainBufferCount;
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
	RtvHeapDesc.NumDescriptors = 2;
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
}

void D3D12Device::Resize(
	uint32 Width,
	uint32 Height)
{
	WaitForGPU();
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));

	IRHIDevice::Resize(Width, Height);

	SwapChainBuffer.resize(SwapChainBufferCount);
	for (uint32 Index = 0; Index < SwapChainBufferCount; ++Index)
	{
		SwapChainBuffer[Index].Reset();
	}
	ThrowIfFailed(SwapChain->ResizeBuffers(
		SwapChainBufferCount,
		WindowWidth, WindowHeight,
		TextureFormatMap[(uint32)BackBufferFormat],
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	//create backbuffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHeapHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (uint32 Index = 0; Index < SwapChainBufferCount; ++Index)
	{
		ThrowIfFailed(SwapChain->GetBuffer(Index, IID_PPV_ARGS(&SwapChainBuffer[Index])));
		ThrowIfFailed(SwapChainBuffer[Index]->SetName(L"BackBuffer"));
		Device->CreateRenderTargetView(SwapChainBuffer[Index].Get(), nullptr, RtvHeapHandle);
		//offset next descriptor buffer
		RtvHeapHandle.Offset(1, RtvDescriptorSize);
	}

	//create depth stencil buffer
	DepthStencilBuffer.Reset();
	D3D12_RESOURCE_DESC DepthStencilDesc;
	DepthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	DepthStencilDesc.Alignment = 0;
	DepthStencilDesc.Width = WindowWidth;
	DepthStencilDesc.Height = WindowHeight;
	DepthStencilDesc.DepthOrArraySize = 1;
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.Format = TextureFormatMap[(uint32)DepthStencilFormat];
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_CLEAR_VALUE ClearValue;
	ClearValue.Format = TextureFormatMap[(uint32)DepthStencilFormat];
	ClearValue.DepthStencil.Depth = 1.f;
	ClearValue.DepthStencil.Stencil = 0;
	CD3DX12_HEAP_PROPERTIES DefaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(Device->CreateCommittedResource(
		&DefaultHeap,
		D3D12_HEAP_FLAG_NONE,
		&DepthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&ClearValue,
		IID_PPV_ARGS(&DepthStencilBuffer)));

	Device->CreateDepthStencilView(
		DepthStencilBuffer.Get(),
		nullptr,
		DepthStencilView());

	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);
	CommandList->ResourceBarrier(
		1,
		&Barrier);

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

ComPtr<ID3D12Resource> D3D12Device::CurrentBackBuffer() const
{
	return SwapChainBuffer[CurrentBackBufferIndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Device::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		RtvHeap->GetCPUDescriptorHandleForHeapStart(), 
		CurrentBackBufferIndex, 
		RtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12Device::DepthStencilView() const
{
	return DsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3D12Device::InitPlatformDenpendentMap()
{
	TextureFormatMap[(uint32)ETextureFormat::TF_Unknown]          = DXGI_FORMAT_UNKNOWN;
	TextureFormatMap[(uint32)ETextureFormat::TF_R16]              = DXGI_FORMAT_R16_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R16G16]           = DXGI_FORMAT_R16G16_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R16G16B16A16]     = DXGI_FORMAT_R16G16B16A16_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R32]              = DXGI_FORMAT_R32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R32G32]           = DXGI_FORMAT_R32G32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R32G32B32]        = DXGI_FORMAT_R32G32B32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R32G32B32A32]     = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_R8G8B8A8]         = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureFormatMap[(uint32)ETextureFormat::TF_D24S8]            = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureFormatMap[(uint32)ETextureFormat::TF_D32]              = DXGI_FORMAT_D32_FLOAT;
	TextureFormatMap[(uint32)ETextureFormat::TF_D32S8]            = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	CullModeMap[(uint32)ECullMode::CM_None]                       = D3D12_CULL_MODE_NONE;
	CullModeMap[(uint32)ECullMode::CM_Back]                       = D3D12_CULL_MODE_BACK;
	CullModeMap[(uint32)ECullMode::CM_Front]                      = D3D12_CULL_MODE_FRONT;

	FillModeMap[(uint32)EFillMode::FM_Wireframe]                  = D3D12_FILL_MODE_WIREFRAME;
	FillModeMap[(uint32)EFillMode::FM_Solid]                      = D3D12_FILL_MODE_SOLID;

	PrimitiveTopologyMap[(uint32)EPrimitiveTopology::PT_Point]    = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	PrimitiveTopologyMap[(uint32)EPrimitiveTopology::PT_Line]     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	PrimitiveTopologyMap[(uint32)EPrimitiveTopology::PT_Triangle] = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	BlendModeMap[(uint32)EBlendMode::BM_One]                      = D3D12_BLEND_ONE;
	BlendModeMap[(uint32)EBlendMode::BM_Zero]                     = D3D12_BLEND_ZERO;
	BlendModeMap[(uint32)EBlendMode::BM_SrcColor]                 = D3D12_BLEND_SRC_COLOR;
	BlendModeMap[(uint32)EBlendMode::BM_InverseSrcColor]          = D3D12_BLEND_INV_SRC_COLOR;
	BlendModeMap[(uint32)EBlendMode::BM_SrcAlpha]                 = D3D12_BLEND_SRC_ALPHA;
	BlendModeMap[(uint32)EBlendMode::BM_InverseSrcAlpha]          = D3D12_BLEND_INV_SRC_ALPHA;
	BlendModeMap[(uint32)EBlendMode::BM_DestColor]                = D3D12_BLEND_DEST_COLOR;
	BlendModeMap[(uint32)EBlendMode::BM_InverseDestColor]         = D3D12_BLEND_INV_DEST_COLOR;
	BlendModeMap[(uint32)EBlendMode::BM_DestAlpha]                = D3D12_BLEND_DEST_ALPHA;
	BlendModeMap[(uint32)EBlendMode::BM_InverseDestAlpha]         = D3D12_BLEND_INV_DEST_ALPHA;

	BlendOperatorMap[(uint32)EBlendOperator::BO_Add]              = D3D12_BLEND_OP_ADD;
	BlendOperatorMap[(uint32)EBlendOperator::BO_Sub]              = D3D12_BLEND_OP_SUBTRACT;
	BlendOperatorMap[(uint32)EBlendOperator::BO_ReverseSub]       = D3D12_BLEND_OP_REV_SUBTRACT;
	BlendOperatorMap[(uint32)EBlendOperator::BO_Min]              = D3D12_BLEND_OP_MIN;
	BlendOperatorMap[(uint32)EBlendOperator::BO_Max]              = D3D12_BLEND_OP_MAX;

	ShaderTypeMap[(uint32)EShaderType::ST_Vertex]                 = "vs_5_0";
	ShaderTypeMap[(uint32)EShaderType::ST_Pixel]                  = "ps_5_0";
	ShaderTypeMap[(uint32)EShaderType::ST_Geometry]               = "gs_5_0";
	ShaderTypeMap[(uint32)EShaderType::ST_Computer]               = "cs_5_0";

	WriteMaskMap[(uint32)EWriteMask::WM_R]                        = D3D12_COLOR_WRITE_ENABLE_RED;
	WriteMaskMap[(uint32)EWriteMask::WM_G]                        = D3D12_COLOR_WRITE_ENABLE_GREEN;
	WriteMaskMap[(uint32)EWriteMask::WM_B]                        = D3D12_COLOR_WRITE_ENABLE_BLUE;
	WriteMaskMap[(uint32)EWriteMask::WM_A]                        = D3D12_COLOR_WRITE_ENABLE_ALPHA;
	WriteMaskMap[(uint32)EWriteMask::WM_RGB]                      = (D3D12_COLOR_WRITE_ENABLE)(D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN | D3D12_COLOR_WRITE_ENABLE_BLUE);
	WriteMaskMap[(uint32)EWriteMask::WM_RGBA]                     = D3D12_COLOR_WRITE_ENABLE_ALL;
 
	DepthFunctionMap[(uint32)EDepthFunction::DF_Never]            = D3D12_COMPARISON_FUNC_NEVER;
	DepthFunctionMap[(uint32)EDepthFunction::DF_Less]             = D3D12_COMPARISON_FUNC_LESS;
	DepthFunctionMap[(uint32)EDepthFunction::DF_Equal]            = D3D12_COMPARISON_FUNC_EQUAL;
	DepthFunctionMap[(uint32)EDepthFunction::DF_LessEqual]        = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	DepthFunctionMap[(uint32)EDepthFunction::DF_Greater]          = D3D12_COMPARISON_FUNC_GREATER;
	DepthFunctionMap[(uint32)EDepthFunction::DF_NotEqual]         = D3D12_COMPARISON_FUNC_NOT_EQUAL;
	DepthFunctionMap[(uint32)EDepthFunction::DF_GreaterEqual]     = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	DepthFunctionMap[(uint32)EDepthFunction::DF_Always]           = D3D12_COMPARISON_FUNC_ALWAYS;
}  

void D3D12Device::CreateTextureXD(TextureDesc& Desc, ITexture* Tex)
{
	D3D12Texture* D3DTexture = new D3D12Texture(Desc);
	
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
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
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
	D3D12_HEAP_PROPERTIES Properties;
	Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//set clear color
	D3D12_CLEAR_VALUE ClearValue;
	if (Desc.Format != ETextureFormat::TF_D24S8 && 
		Desc.Format != ETextureFormat::TF_D32 && 
		Desc.Format != ETextureFormat::TF_D32S8)
	{
		FLOAT ClearColor[] = { Desc.ClearValues.ClearColorR, Desc.ClearValues.ClearColorG, Desc.ClearValues.ClearColorB, Desc.ClearValues.ClearColorA };
		ClearValue = CD3DX12_CLEAR_VALUE(TextureFormatMap[(uint32)Desc.Format], ClearColor);
	}
	else
	{
		ClearValue = CD3DX12_CLEAR_VALUE(TextureFormatMap[(uint32)Desc.Format], Desc.ClearValues.ClearDepth, Desc.ClearValues.ClearStencil);
	}
	D3D12_CLEAR_VALUE* CV = Desc.IsClearValid() ? &ClearValue : nullptr;

	//create resource
	ThrowIfFailed(Device->CreateCommittedResource(
		&Properties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		CV,
		IID_PPV_ARGS(&D3DTexture->GetResource())));

	Tex = D3DTexture;
}


IIndexBuffer* D3D12Device::CreateIndexBuffer()
{
	return new D3D12IndexBuffer();
}

IVertexBuffer* D3D12Device::CreateVertexBuffer()
{
	return new D3D12VertexBuffer();
}

IShader* D3D12Device::CreateShader()
{

	return new D3D12Shader();
}

IPipelineStateObject* D3D12Device::CreatePipelineStateObject()
{
	return new D3D12PipelineStateObject();
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

void D3D12Device::BeginRenderPass(RHIRenderPassInfo& RPInfo)
{

}

void D3D12Device::EndRenderPass()
{

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

void D3D12Device::Present() const
{
	SwapChain->Present(1, 0);
}

void D3D12Device::BeginFrame()
{
	ThrowIfFailed(CommandAllocator->Reset());
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));

	float Color[] = { 1.f, 0.f, 0.f, 0.f };
	D3D12_RECT Rect = { 0, 0, (int32)WindowWidth, (int32)WindowHeight };
	CommandList->ClearRenderTargetView(CurrentBackBufferView(), Color, 1, &Rect);
}

void D3D12Device::EndFrame()
{
	CD3DX12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CommandList->ResourceBarrier(1, &Barrier);

	FlushCommandQueue();

	CurrentBackBufferIndex = (CurrentBackBufferIndex + 1) % 2;

	Present();

	WaitForGPU();
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

	CD3DX12_RESOURCE_BARRIER Barrier =
		CD3DX12_RESOURCE_BARRIER::Transition(
			DefaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST);
	CommandList->ResourceBarrier(1, &Barrier);
	UpdateSubresources<1>(CommandList.Get(), DefaultBuffer.Get(), UploadBuffer.Get(), 0, 0, 1, &SubResourceData);

	Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		DefaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	CommandList->ResourceBarrier(1, &Barrier);

	return DefaultBuffer;
}

void D3D12Device::BuildIndexBuffer(IIndexBuffer* IndexBuffer)
{
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));

	D3D12IndexBuffer* D3DIB = (D3D12IndexBuffer*)IndexBuffer;
	D3DIB->IndexBufferGPU = CreateDefaultBuffer(D3DIB->GetData(), D3DIB->GetDataSize(), D3DIB->UploadBuffer);

	ThrowIfFailed(CommandList->Close());
}

void D3D12Device::BuildVertexBuffer(IVertexBuffer* VertexBuffer)
{
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));

	D3D12VertexBuffer* D3DVB = (D3D12VertexBuffer*)VertexBuffer;
	D3DVB->VertexBufferGPU = CreateDefaultBuffer(D3DVB->GetData(), D3DVB->GetDataSize(), D3DVB->UploadBuffer);

	ThrowIfFailed(CommandList->Close());
}

void D3D12Device::BuildShader(IShader* Shader)
{
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));

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

	ThrowIfFailed(CommandList->Close());
}

void D3D12Device::BuildPipelineStateObject(IPipelineStateObject* PSO)
{
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));

	D3D12PipelineStateObject* D3DPSO = (D3D12PipelineStateObject*)PSO;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
	ZeroMemory(&PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	D3D12Shader* VertexShader = (D3D12Shader*)D3DPSO->VertexShader;
	D3D12Shader* PixelShader = (D3D12Shader*)D3DPSO->PixelShader;
	D3D12Shader* GeometryShader = (D3D12Shader*)D3DPSO->GeometryShader;
	std::vector< D3D12_INPUT_ELEMENT_DESC>       VertexLayout;
	VertexLayout = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 } };
	PSODesc.InputLayout = { VertexLayout.data() , 1 };
	PSODesc.pRootSignature = nullptr;
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
	PSODesc.RasterizerState.DepthClipEnable = true;
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
	PSODesc.PrimitiveTopologyType = PrimitiveTopologyMap[(uint32)D3DPSO->PrimitiveTopology];
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

	ThrowIfFailed(CommandList->Close());
}

void D3D12Device::SetPipelineStateObject(IPipelineStateObject* PSO)
{
	D3D12PipelineStateObject* D3DPSO = (D3D12PipelineStateObject*)PSO;
	CommandList->SetPipelineState(D3DPSO->GetPSO().Get());
}

void D3D12Device::DrawIndexedInstanced(IIndexBuffer* IndexBuffer, IVertexBuffer* VertexBuffer)
{
	D3D12IndexBuffer* D3DIB = (D3D12IndexBuffer*)IndexBuffer;
	D3D12VertexBuffer* D3DVB = (D3D12VertexBuffer*)VertexBuffer;
	D3D12_INDEX_BUFFER_VIEW IndexBufferView = D3DIB->GetIndexBufferView();
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView = D3DVB->GetVertexBufferView();
	CommandList->IASetIndexBuffer(&IndexBufferView);
	CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	CommandList->DrawIndexedInstanced(IndexBuffer->GetIndexCount(), 1, 0, 0, 0);
}