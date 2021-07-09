#include <stdafx.h>
#include "D3D12FrameResource.h"

void D3D12FrameResource::ResetOnDeviceLost()
{
	CommandAllocator.Reset();
}
