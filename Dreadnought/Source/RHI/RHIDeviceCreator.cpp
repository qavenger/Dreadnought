#include "stdafx.h"
#include "RHIDeviceCreator.h"
#include "D3D12/D3D12Device.h"

IRHIDevice* RHIDeviceCreator::Device = nullptr;
void RHIDeviceCreator::Init()
{
#ifdef USE_DX12_RHI
	Device = new D3D12Device();
#elif USE_OPENGL_RHI
	Device = new OpenglDevice();
#endif
}