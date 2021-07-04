#pragma once
#include <set>

template<typename... Args>
class TMulticastDelegate
{
public:
#define Arg(x) void(*x)(Args...)
	void Bind(Arg(f)) { delegs.insert(f); }
	void Unbind(Arg(f)) { delegs.erase(f); }
	void Broadcast(Args... args) { if (!delegs.empty()) for (auto f : delegs) f(args...); }
	void UnbindAll() { delegs.clear(); }
	bool IsBound(Arg(f))const { auto it = delegs.find(f); return it != delegs.end(); }
#undef Arg
private:

	std::set<void(*)(Args...)> delegs;
};