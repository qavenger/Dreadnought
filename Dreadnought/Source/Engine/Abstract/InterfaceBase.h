#pragma once
class IInterface
{
};

class INoncopyable : public IInterface
{
protected:
	INoncopyable()noexcept {}
	~INoncopyable() {}
private:
	INoncopyable(const INoncopyable&) = delete;
	INoncopyable operator=(const INoncopyable&) = delete;
};

class ISubsystem : public INoncopyable
{
	friend class Engine;
public:
	static ISubsystem* GetSubsystem(const tchar* name);
protected:
	virtual ~ISubsystem() {}
	virtual void OnInit() = 0;
	virtual void OnDestroy() = 0;
	virtual void OnPreTick(float dt) {};
	virtual void OnTick(float dt) {};
	virtual void OnPostTick(float dt) {};
};