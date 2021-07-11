#pragma once

class IRHIDevice;
class RHIDeviceCreator
{
public:
	static void Init();

public:
	static IRHIDevice* Device;
};