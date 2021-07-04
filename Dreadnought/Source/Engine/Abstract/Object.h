#pragma once
class IObject
{
	friend class Engine;
public:

protected:
	virtual void OnDestroy();
	IObject();
	virtual ~IObject(){}

private:
	uint8 bPendingKill : 1;
};