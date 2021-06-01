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

void Graphics::OnResize(uint width, uint height, bool minimized)
{
	IsFullyInited = DeviceResources->OnResize(width, height, minimized);
}

void Graphics::OnDestroy()
{
}

void Graphics::Resize(int resolutionIndex, EWindowMode resizeMode)
{
	uint r = DeviceResources->GetSelectedResolution();
	uint w = r & 0x0000ffff, h = (r >> 16);
	RECT rect;
	Window::GetInstance()->SetDimension(w, h, resizeMode);
	GetClientRect(Window::GetInstance()->GetHandle(), &rect);
	OnResize(rect.right - rect.left, rect.bottom - rect.top, EWindowMode::MINIMIZED == resizeMode);

}

void Graphics::Resize(uint width, uint height, EWindowMode resizeMode)
{
	Window::GetInstance()->SetDimension(width, height, resizeMode);
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

void Graphics::OnDeviceLost()
{
	IsFullyInited = false;
}

void Graphics::OnDeviceRestored()
{
	IsFullyInited = true;
}
