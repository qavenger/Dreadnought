#pragma once

class IRHIDevice;
class RHIDeviceCreator
{
public:
	static void Init();
	static void Destroy();
public:
	static IRHIDevice* Device;
};