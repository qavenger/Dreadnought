#include <stdafx.h>
#include "InterfaceBase.h"
#include <Engine.h>

ISubsystem* ISubsystem::GetSubsystem(const tchar* name)
{
	return gEngine->GetSubsystem(name);
}
