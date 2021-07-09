#pragma once
#ifdef PLATFORM_WINDOWS
#define HEADER "RHI/D3D12/D3D12DeviceResource.h"
#endif
#ifdef HEADER
#include HEADER
#endif

class RHIDevice : public IDeviceNotify, public ISubsystem
{
public:
	RHIDevice();
protected:
	std::unique_ptr<D3D12DeviceResource> DeviceResources;
	virtual void OnPostTick(float dt);
	// Inherited via IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	// Inherited via ISubsystem
	virtual void OnInit() override;
	virtual void OnDestroy() override;
};

