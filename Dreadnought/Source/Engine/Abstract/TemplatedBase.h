#pragma once
#include <set>
template<typename RetType, typename... Args>
class TFunctionBase
{
public:
	virtual RetType operator()(Args... args) = 0;
	virtual bool IsEqual(const TFunctionBase<RetType, Args...>* other)const = 0;
};

template<typename T, typename RetType, typename... Args>
class TFunction : public TFunctionBase<RetType, Args...>
{
public:
	virtual bool IsEqual(const TFunctionBase<RetType, Args...>* other)const
	{
		T* otherFunc = (T*)other;
		T* thisFunc = (T*)(this);
		if (otherFunc)
		{
			return (*thisFunc)==(*otherFunc);
		}
		return false;
	}
};

template<typename Class, typename RetType, typename... Args>
class TMemberFunction : public TFunction<TMemberFunction<Class, RetType, Args...>,RetType, Args...>
{
public:
	TMemberFunction(Class* pObj, RetType(Class::* f)(Args...))
		: pObj(pObj), pFunc(f){}
	virtual RetType operator()(Args... args)override final
	{
		(pObj->*pFunc)(args...);
	}

	bool operator==(const TMemberFunction& other)const
	{
		return pObj == other.pObj && pFunc == other.pFunc;
	}

	bool operator!=(const TMemberFunction& other)const
	{
		return pObj != other.pObj || pFunc != other.pFunc;
	}
protected:
	Class* pObj;
	RetType(Class::* pFunc)(Args...);
};

template<typename RetType, typename... Args>
class TFunctionRaw : public TFunction<TFunctionRaw<RetType, Args...>, RetType, Args...>
{
public:
	TFunctionRaw(RetType(*pFunc)(Args...)) :pFunc(pFunc) {}
	virtual RetType operator()(Args... args)override final
	{
		pFunc(args...);
	}

	bool operator==(const TFunctionRaw& other)const
	{
		return other.pFunc == pFunc;
	}

	bool operator!=(const TFunctionRaw& other)const
	{
		return other.pFunc != pFunc;
	}
protected:
	RetType(*pFunc)(Args...);
};

template<typename... Args>
class TMulticastDelegate
{
	using RawFunctionPtrType = TFunctionRaw<void, Args...>;
public:
	~TMulticastDelegate()
	{
		UnbindAll();
	}
	template<typename T, typename F>
	void Bind(T* pObj, F pFunc)
	{
		auto* p = FindMemberFunction(pObj, pFunc);
		if (p == nullptr)
		{
			pFuncs.insert(new TMemberFunction<T, void, Args...>(pObj, pFunc));
		}
	}

	void Bind(void(*pFunc)(Args... args))
	{
		auto* p = FindRawFunction(pFunc);
		if (p == nullptr)
		{
			pFuncs.insert(new TFunctionRaw<void, Args...>(pFunc));
		}
	}

	template<typename T, typename F>
	void Unbind(T* pObj, F pFunc)
	{
		auto* p = FindMemberFunction(pObj, pFunc);
		if (p)
		{
			pFuncs.erase(p);
			delete p;
		}
	}

	void Unbind(void(*pFunc)(Args... args))
	{
		auto* p = FindRawFunction(pFunc);
		if (p)
		{
			pFuncs.erase(p);
			delete p;
		}
	}

	void Broadcast(Args... args)
	{
		if(!pFuncs.empty())
			for (auto* f : pFuncs)
			{
				(*f)(args...);
			}
	}

	template<typename T, typename F>
	bool IsBound(T* pObj, F pFunc)const
	{
		auto* p = FindMemberFunction(pObj,pFunc);
		return (p != nullptr);
	}

	bool IsBound(void(*pFunc)(Args...))const
	{
		auto* p = FindRawFunction(pFunc);
		return (p != nullptr);
	}

	void UnbindAll()
	{
		for (auto* f : pFuncs)
		{
			delete f;
		}
		pFuncs.clear();
	}
private:
	TFunctionBase<void, Args...>* FindRawFunction(void(*pFunc)(Args...))
	{
		TFunctionRaw<void, Args...> raw(pFunc);
		for (auto* f : pFuncs)
		{
			if (f->IsEqual(&raw))
			{
				return f;
			}
		}
		return nullptr;
	}
	template<typename T, typename F>
	TFunctionBase<void, Args...>* FindMemberFunction(T* pObj, F pFunc)
	{
		TMemberFunction<T, void, Args...> memberFunc(pObj, pFunc);
		for (auto* f : pFuncs)
		{
			if (f->IsEqual(&memberFunc))
			{
				return f;
			}
		}
		return nullptr;
	}

	const TFunctionBase<void, Args...>* FindRawFunction(void(*pFunc)(Args...))const
	{
		TFunctionRaw<void, Args...> raw(pFunc);
		for (auto* f : pFuncs)
		{
			if (f->IsEqual(&raw))
			{
				return f;
			}
		}
		return nullptr;
	}
	template<typename T, typename F>
	const TFunctionBase<void, Args...>* FindMemberFunction(T* pObj, F pFunc)const
	{
		TMemberFunction<T, void, Args...> memberFunc(pObj, pFunc);
		for (auto* f : pFuncs)
		{
			if (f->IsEqual(&memberFunc))
			{
				return f;
			}
		}
		return nullptr;
	}
private:
	std::set< TFunctionBase<void, Args...>* > pFuncs;
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

//template<typename... Args>
//class TMulticastDelegate
//{
//public:
//#define Arg(x) void(*x)(Args...)
//	void Bind(Arg(f)) { delegs.insert(f); }
//	void Unbind(Arg(f)) { delegs.erase(f); }
//	void Broadcast(Args... args) { if (!delegs.empty()) for (auto f : delegs) f(args...); }
//	void UnbindAll() { delegs.clear(); }
//	bool IsBound(Arg(f))const { auto it = delegs.find(f); return it != delegs.end(); }
//#undef Arg
//private:
//
//	std::set<void(*)(Args...)> delegs;
//};