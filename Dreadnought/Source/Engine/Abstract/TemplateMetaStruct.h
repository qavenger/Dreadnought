#pragma once
template<typename T>
struct TTypeIs { using type = T; };

template<typename T, T Value>
struct TIntegralConstant {
	using value_type = T;
	using type = TIntegralConstant;
	static constexpr T value = Value;
};

template<bool Value>
using TBoolType = TIntegralConstant<bool, Value>;

using TrueType = TBoolType<true>;
using FalseType = TBoolType<false>;

template<bool Predicate, typename T>
struct TEnableIf {};

template<typename T>
struct TEnableIf<true, T>:TTypeIs<T>{};

template<typename T>
struct TRemoveConst {
	using type = T;
};

template<typename T>
struct TRemoveConst<const T> {
	using type = T;
};

template<typename T>
struct TRemoveVolatile {
	using type = T;
};

template<typename T>
struct TRemoveVolatile<volatile T> {
	using type = T;
};

template<typename T>
struct TRemoveCV {
	using type = typename TRemoveConst< typename TRemoveVolatile<T>::type >::type;
};

template<typename T>
struct TRank{
	static constexpr size_t value = 0;
};

template<typename T>
struct TRank<T[]> {
	static constexpr size_t value = 1 + TRank<T>::value;
};

template<typename T>
struct TRank<T(&)[]> {
	static constexpr size_t value = 1 + TRank<T>::value;
};

template<typename T, size_t N>
struct TRank<T[N]> {
	static constexpr size_t value = 1 + TRank<T>::value;
};

template<typename T, size_t N>
struct TRank<T(&)[N]> {
	static constexpr size_t value = 1 + TRank<T>::value;
};

template<typename...>
using TVoidType = void;

template<typename T>
struct TIsArray : TBoolType < TRank<T>::value >= 1 > {};

template<typename T>
struct TIsRef : FalseType{};

template<typename T>
struct TIsRef<T&> : TrueType {};

template<typename T0, typename T1>
struct TIsSameType : FalseType{};

template<typename T0>
struct TIsSameType<T0, T0> :TrueType {};

template<typename T>
struct TRemoveRef {
	using type = T;
};

template<typename T>
struct TRemoveRef<T&>{
	using type = T;
};

template<typename T>
struct TRemoveRef<T&&>{
	using type = T;
};

template<typename T>
struct TRemoveArray {
	using type = T;
};

template<typename T>
struct TRemoveArray<T[]> {
	using type = typename TRemoveArray<T>::type;
};

template<typename T>
struct TRemoveArray<T(&)[]> {
	using type = typename TRemoveArray<T>::type;
};

template<typename T, size_t N>
struct TRemoveArray<T[N]> {
	using type = typename TRemoveArray<T>::type;
};

template<typename T, size_t N>
struct TRemoveArray<T(&)[N]> {
	using type = typename TRemoveArray<T>::type;
};

template<typename T>
struct TRemovePointer{
	using type = T;
};

template<typename T>
struct TRemovePointer<T*> {
	using type = typename TRemovePointer<T>::type;
};

template<typename T, typename = void>
struct TAddPointer {
	using type = T;
};

template<typename T>
struct TAddPointer<T, TVoidType< typename TRemoveRef<T>::type* >>
{
	using type = typename TRemoveRef<T>::type*;
};

template<bool Predicate, typename T0, typename T1>
struct TSelect {
	using type = T0;
};

template<typename T0, typename T1>
struct TSelect<false, T0, T1>{
	using type = T1;
};

template<typename T, typename = void>
struct TIsFunction : TrueType {};

template<typename T>
struct TIsFunction<T, TVoidType<decltype(sizeof(T))>> : FalseType {};

template<typename T>
struct TDecay
{
	using type_noRef = typename TRemoveRef<T>::type;
	using type_noCV = typename TSelect<TIsFunction<T>::value, typename TAddPointer<type_noRef>::type , typename TRemoveCV<type_noRef>::type>::type;
	using type = typename TSelect< TIsArray<T>::value, typename TAddPointer<typename TRemoveArray<type_noRef>::type>::type, type_noCV >::type;
};

template<typename T, typename... Rest>
struct TIsA : FalseType{};

template<typename T, typename T1, typename... Rest>
struct TIsA<T, T1, Rest...> {
	static constexpr bool value = TSelect< TIsSameType<T, T1>::value, TrueType, TIsA<T, Rest...> >::type::value;
};

template<typename T, typename T1>
struct TIsA<T, T1> : TIsSameType<T, T1> {};

template<typename T, typename T1>
struct TIsArrayOrRefOfType
{
	static constexpr bool value = 
		(TIsArray<T>::value | TIsRef<T>::value) && 
		TIsSameType<
			typename TRemoveCV< T1 >::type, 
			typename TRemoveCV< typename TRemoveRef<typename TRemoveArray<T>::type>::type >::type
		>::value;
};

template<typename T, typename =void>
struct TIsClass : FalseType{};

template<typename T>
struct TIsClass < T, TVoidType<char T::*>> : TrueType {};