#pragma once
#include <set>
template<typename RetType, typename... Args>
class TFunctionBase
{
public:
	virtual RetType operator()(Args...) = 0;
};

template<typename Class, typename RetType, typename... Args>
class TMemberFunction : public TFunctionBase<RetType, Args...>
{
public:
	TMemberFunction(Class* pObj, RetType(Class::* f)(Args...))
		: pObj(pObj), pFunc(f){}
	virtual RetType operator()(Args... args)override final
	{
		(pObj->*pFunc)(args...);
	}
protected:
	Class* pObj;
	RetType(Class::* pFunc)(Args...);
};

template<typename RetType, typename... Args>
class TFunction : public TFunctionBase<RetType, Args...>
{
public:
	TFunction(RetType(*pFunc)(Args...)) :pFunc(pFunc) {}
	virtual RetType operator()(Args... args)override final
	{
		pFunc(args...);
	}
protected:
	RetType(*pFunc)(Args...);
};

//template<size_t NumArgs, typename...Args>
//class TMulticastDelegate
//{
//public:
//	static constexpr size_t ArgCount = sizeof ...(Args);
//	TMulticastDelegate() { static_assert(NumArgs == ArgCount); }
//	~TMulticastDelegate() { 
//		for (auto o : pFuncs) 
//		{ 
//			delete o; 
//		} 
//		pFuncs.clear(); 
//	}
//	template<size_t i>
//	struct ArgTypes
//	{
//		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
//	};
//
//#define Arg(x) void(*x)(Args...)
//	void Bind(Arg(f)) { pFuncs.insert(new TFunction(f)); }
//#undef Arg
//private:
//	std::set< TFunctionBase<void, Args...>* > pFuncs;
//};

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