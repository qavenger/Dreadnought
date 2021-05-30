#pragma once

#include "D3D12/D3D12DeviceResource.h"

class Window;
#define NUM_FRAMEBUFFER 2
class Graphics : public IDeviceNotify, public INoncopyable
{
public:
	virtual ~Graphics();
public:
	static Graphics* GetInstance();
	virtual bool Init() = 0;
	virtual void OnResize(uint width, uint height, bool minimized) = 0;
	virtual void OnDestroy() = 0;
public:
	void Resize(int resolutionIndex, EWindowMode resizeMode = EWindowMode::WINDOW);
	void Resize(uint width, uint height, EWindowMode resizeMode = EWindowMode::WINDOW);
	void Tick();
	virtual void OnRender() = 0;
public:
	bool IsInited()const { return IsFullyInited; }
protected:
	std::shared_ptr<D3D12DeviceResource> DeviceResources;
	// Inherited via IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;
	static void SetInstance(Graphics* instance) { Instance = instance; }
private:
	bool IsFullyInited = false;
	static Graphics* Instance;
};