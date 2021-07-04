#include <stdafx.h>
#include "RHIDevice.h"
#include "UserInterface/Console.h"

RHIDevice::RHIDevice()
{
	DeviceResources = std::make_unique<D3D12DeviceResource>();
	DeviceResources->RegisterDeviceNotify(this);
	DeviceResources->InitDXGIAdapter();
}

void RHIDevice::OnPostTick(float dt)
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	DeviceResources->BeginFrame();

	if (gEngine->GetShowConsole())
	{
		Console::GetInstance().Draw("Console", &gEngine->GetShowConsole());
	}

	DeviceResources->PrepareGUI();
	ImGui::Render();

	float color[] = { (float)sin(Engine::GetGameTime()),0,0,1 };
	DeviceResources->ClearRenderTargetView(color);

	D3D12_CPU_DESCRIPTOR_HANDLE handle[] = { DeviceResources->GetRenderTargetView() };
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DeviceResources->GetDepthStencilView();
	DeviceResources->GraphicsCommandList->OMSetRenderTargets(ARRAYSIZE(handle),
		handle,
		TRUE,
		&dsvHandle
	);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DeviceResources->GraphicsCommandList.Get());
	DeviceResources->EndFrame();
}

void RHIDevice::OnDeviceLost()
{
	
}

void RHIDevice::OnDeviceRestored()
{
}

void RHIDevice::OnInit()
{
	DeviceResources->CreateDeviceResources();
	DeviceResources->CreateViewDependentResources();
}

void RHIDevice::OnDestroy()
{
	DeviceResources = nullptr;
	ImGui_ImplDX12_Shutdown();
}
