#pragma once

#include "RHI/RHIDevice.h"

class DeferredRenderer : public ISubsystem
{
protected:
	virtual void OnPostTick(float dt);

	// Inherited via ISubsystem
	virtual void OnInit() override;
	virtual void OnDestroy() override;

private:
};