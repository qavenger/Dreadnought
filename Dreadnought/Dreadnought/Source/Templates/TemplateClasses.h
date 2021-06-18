#pragma once
class INoncopyable
{
protected:
	INoncopyable() {}
	~INoncopyable() {}
private:
	INoncopyable(const INoncopyable&) = delete;
	INoncopyable operator=(const INoncopyable&) = delete;
};

template<typename... Args>
class TMulticastDelegate
{
public:
	void Bind(void (*f)(Args...)) { delegs.insert(f); }
	void UnBind(void (*f)(Args...)) { delegs.erase(f); }
	void Broadcast(Args... args) { if (!delegs.empty())for (auto f : delegs) { f(args...); } }
	bool IsBound(void (*f)(Args...)) { auto it = delegs.find(f); return it != delegs.end(); }
private:
	std::set<void (*)(Args...)> delegs;
};
