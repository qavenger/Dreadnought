#include "pch.h"
#include <Graphics/Graphics.h>
#include <Window/Window.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#define THROW_IF_FAILED(rs, msg) { HRESULT hr; if(FAILED(hr = rs)) {PrintDebugMessage(msg); return hr;}}
#define ASSERT_IF_FAILED(rs) assert(SUCCEEDED(rs));

Graphics* Graphics::Instance = nullptr;

Graphics::~Graphics()
{}

Graphics* Graphics::GetInstance()
{
	return Instance;
}

bool Graphics::Init()
{
	DeviceResources = std::make_unique<D3D12DeviceResource>(
		DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		2
		);
	DeviceResources->RegisterDeviceNotify(this);
	DeviceResources->InitDXGIAdapter();
	DeviceResources->CreateDeviceResources();

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		ThrowIfFailed(DeviceResources->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&SRVDescriptorHeap_GUI)));
	}
	return true;
}

RECT Graphics::FindBestOutput(uint& outputIndex)const
{
	RECT coord = {};
	uint best = 0;
	uint bestIdx = 0;
	for (auto output : DeviceResources->Outputs)
	{
		DXGI_OUTPUT_DESC desc;
		ThrowIfFailed(output->GetDesc(&desc));
		RECT cRect = desc.DesktopCoordinates;
		uint area = (cRect.right - cRect.left) * (cRect.bottom - cRect.top);
		if (area > best)
		{
			coord = cRect;
			outputIndex = bestIdx;
		}
		++bestIdx;
	}
	
	return coord;
}

void Graphics::OnResize(uint width, uint height, bool minimized)
{
	IsFullyInited = DeviceResources->OnResize(width, height, minimized);
}

void Graphics::OnDestroy()
{
}

void Graphics::Resize(RECT rect, EWindowMode resizeMode)
{
	Window::GetInstance()->SetDimension(rect, resizeMode);
	GetClientRect(Window::GetInstance()->GetHandle(), &rect);
	OnResize(rect.right - rect.left, rect.bottom - rect.top, EWindowMode::MINIMIZED == resizeMode);
}

#include "Timer.h"

void Graphics::Tick()
{
	if (!DeviceResources->IsVisiable())
	{
		return;
	}

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	DeviceResources->BeginFrame();
	OnRender();
	DeviceResources->GetCommandList()->SetDescriptorHeaps(1, SRVDescriptorHeap_GUI.GetAddressOf());
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DeviceResources->GetCommandList());
	DeviceResources->EndFrame();
}

RECT Graphics::FindBestOutput(RECT rect, uint& outputIndex) const
{
	uint bestIdx = 0;
	RECT coord = {};
	for (auto output : DeviceResources->Outputs)
	{
		DXGI_OUTPUT_DESC desc;
		ThrowIfFailed(output->GetDesc(&desc));
		RECT cRect = desc.DesktopCoordinates;
		if (rect.left > cRect.left && rect.left < cRect.right && rect.top > cRect.top && rect.top < cRect.bottom)
		{
			outputIndex = bestIdx;
			coord = cRect;
		}
		++bestIdx;
	}
	return coord;
}

RECT Graphics::GetOutputRect(uint outputIndex) const
{
	assert(outputIndex < DeviceResources->Outputs.size() && outputIndex >= 0);
	DXGI_OUTPUT_DESC desc;
	DeviceResources->Outputs[outputIndex]->GetDesc(&desc);
	return desc.DesktopCoordinates;
}

void Graphics::OnDeviceLost()
{
	IsFullyInited = false;
}

void Graphics::OnDeviceRestored()
{
	IsFullyInited = true;
}
