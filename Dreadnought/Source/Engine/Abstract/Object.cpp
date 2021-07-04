#include <stdafx.h>
#include <Engine.h>

void IObject::OnDestroy()
{
	bPendingKill = true;
}

IObject::IObject()
	: bPendingKill(false)
{
	
}
