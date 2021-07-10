#include <stdafx.h>
#include "D3D12ViewResource.h"
#include "D3D12DeviceResource.h"

namespace
{
	inline DXGI_FORMAT NoSRGB(DXGI_FORMAT InFormat)
	{
		switch (InFormat)
		{
		case DXGI_FORMAT_BC1_UNORM_SRGB:		return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_UNORM_SRGB:		return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_UNORM_SRGB:		return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_BC7_UNORM_SRGB:		return DXGI_FORMAT_BC7_UNORM;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:	return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:	return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:	return DXGI_FORMAT_B8G8R8X8_UNORM;
		default:								return InFormat;
		}
	}
}

D3D12ViewResource::D3D12ViewResource(D3D12DeviceResource* DeviceContext, Window* window, bool bNeedGUI)
	:
	pDeviceContext(DeviceContext),
	pWindow(window)
{
	FatalIfFalse(pDeviceContext != nullptr && pWindow != nullptr, "Create D3D12 View Resource Failed");
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.NumDescriptors = pDeviceContext->GetBackBufferCount();
	FatalIfFailed(pDeviceContext->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&RTVHeap)));

	if (pDeviceContext->GetDepthStencilFormat() != DXGI_FORMAT_UNKNOWN)
	{
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = 1;
		FatalIfFailed(pDeviceContext->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&DSVHeap)));
	}

	if (bNeedGUI)
	{
		ImGui_ImplWin32_Init(pWindow->GetWindowHandle());
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		FatalIfFailed(pDeviceContext->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&SRVHeap_GUI)));


		ImGui_ImplDX12_Init(
			pDeviceContext->GetDevice(),
			pDeviceContext->GetBackBufferCount(),
			pDeviceContext->GetBackBufferFormat(),
			SRVHeap_GUI.Get(),
			SRVHeap_GUI->GetCPUDescriptorHandleForHeapStart(),
			SRVHeap_GUI->GetGPUDescriptorHandleForHeapStart()
		);
	}
}

D3D12ViewResource::~D3D12ViewResource()
{
	Reset();
	pDeviceContext = nullptr;
	pWindow = nullptr;
}

void D3D12ViewResource::ClearRenderTargetView(ID3D12GraphicsCommandList* CommandList, float color[4])
{
	CommandList->ClearRenderTargetView(pDeviceContext->GetRenderTargetView(), color, 1, &pWindow->GetRect());
}

void D3D12ViewResource::ClearDepthStencilView(ID3D12GraphicsCommandList* CommandList)
{
	CommandList->ClearDepthStencilView(pDeviceContext->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1, 0, 1, &pWindow->GetRect());
}

ID3D12Resource* D3D12ViewResource::GetRenderTarget(uint32 Index)const
{
	return RenderTargets[Index].Get();
}

uint32 D3D12ViewResource::GetCurrentBackBufferIndex()const
{
	return SwapChain->GetCurrentBackBufferIndex();
}

void D3D12ViewResource::Resize()
{
	pDeviceContext->WaitForGPU();
	ImGui_ImplDX12_InvalidateDeviceObjects();

	uint32 w = pWindow->GetWidth();
	uint32 h = pWindow->GetHeight();
	uint32 numBackBuffer = pDeviceContext->GetBackBufferCount();
	DXGI_FORMAT backBufferFormat = NoSRGB(pDeviceContext->GetBackBufferFormat());
	for (uint32 i = 0; i < numBackBuffer; ++i)
	{
		RenderTargets[i].Reset();
	}

	if (SwapChain)
	{
		ThrowIfFailed(SwapChain->ResizeBuffers(numBackBuffer, w, h, backBufferFormat, 0));
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = w;
		desc.Height = h;
		desc.Format = backBufferFormat;
		desc.BufferCount = numBackBuffer;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
		fullScreenDesc.Windowed = true;

		ComPtr<IDXGISwapChain1> swapChain;
		FatalIfFailed(pDeviceContext->GetIDXGIFactory()->CreateSwapChainForHwnd(pDeviceContext->GetRenderCommandQueue(), pWindow->GetWindowHandle(), &desc, &fullScreenDesc, nullptr, &swapChain));
		FatalIfFailed(swapChain.As(&SwapChain));
	}

	for (uint32 i = 0; i < numBackBuffer; ++i)
	{
		FatalIfFailed(SwapChain->GetBuffer(i, IID_PPV_ARGS(&RenderTargets[i])));
		auto* currentRT = RenderTargets[i].Get();
#if defined(_DEBUG) || defined(DEBUG)
		std::wstring name = L"Render Target ";
		name += i;
		ThrowIfFailed(currentRT->SetName(name.c_str()));
#endif

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = backBufferFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(RTVHeap->GetCPUDescriptorHandleForHeapStart(), i, pDeviceContext->GetRTVDescriptorSize());
		pDeviceContext->GetDevice()->CreateRenderTargetView(currentRT, &rtvDesc, handle);

		if (DXGI_FORMAT_UNKNOWN != pDeviceContext->GetDepthStencilFormat())
		{
			CD3DX12_HEAP_PROPERTIES dsvProps(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(pDeviceContext->GetDepthStencilFormat(), w, h, 1, 1);
			desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE depthClear = {};
			depthClear.DepthStencil.Depth = 1;
			depthClear.DepthStencil.Stencil = 1;
			depthClear.Format = pDeviceContext->GetDepthStencilFormat();

			FatalIfFailed(pDeviceContext->GetDevice()->CreateCommittedResource(&dsvProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClear, IID_PPV_ARGS(&DepthStencilBuffer)));
		#if defined(_DEBUG) || defined(DEBUG)
			std::wstring name = L"Depth Stencil";
			ThrowIfFailed(DepthStencilBuffer->SetName(name.c_str()));
		#endif
			D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
			dsv.Format = pDeviceContext->GetDepthStencilFormat();
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			pDeviceContext->GetDevice()->CreateDepthStencilView(DepthStencilBuffer.Get(), &dsv, DSVHeap->GetCPUDescriptorHandleForHeapStart());
		}
	}
	ImGui_ImplDX12_CreateDeviceObjects();
}


void D3D12ViewResource::Reset()
{
	for (uint32 i = 0; i < pDeviceContext->GetBackBufferCount(); ++i)
	{
		RenderTargets[i].Reset();
	}
	DepthStencilBuffer.Reset();
	RTVHeap.Reset();
	DSVHeap.Reset();
	SRVHeap_GUI.Reset();
	SwapChain.Reset();
}

HRESULT D3D12ViewResource::Present()
{
	return SwapChain->Present(1, 0);
}
