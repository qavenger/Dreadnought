#include "pch.h"
#include <Graphics/Graphics.h>
#include <Window/Window.h>

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
	DeviceResources->BeginFrame();
	
	OnRender();

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
