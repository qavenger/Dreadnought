#include "stdafx.h"
#include "RHIDevice.h"

void IRHIDevice::Resize(
	uint32 Width,
	uint32 Height)
{
	WindowWidth = Width;
	WindowHeight = Height;
}

void IRHIDevice::CreateSwapChain(
	HWND Handle,
	uint32 Width,
	uint32 Height,
	uint32 BufferCount,
	ETextureFormat BBFormat,
	ETextureFormat DSFormat)
{

	WindowHandle = Handle;
	WindowWidth = Width;
	WindowHeight = Height;
	BackBufferCount = BufferCount;
	BackBufferFormat = BBFormat;
	DepthStencilFormat = DSFormat;
	CurrentBackBufferIndex = 0;
}