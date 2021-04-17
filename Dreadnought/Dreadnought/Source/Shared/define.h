#pragma once
typedef unsigned char uint8;
typedef char int8;
typedef unsigned int uint;
typedef unsigned short uint16;
typedef short		int16;
typedef unsigned long long uint64;
typedef long long	int64;
typedef DirectX::XMFLOAT2 float2;
typedef DirectX::XMFLOAT3 float3;
typedef DirectX::XMFLOAT4 float4;
typedef DirectX::XMINT2 int2;
typedef DirectX::XMINT3 int3;
typedef DirectX::XMINT4 int4;
typedef DirectX::XMUINT2 uint2;
typedef DirectX::XMUINT3 uint3; 
typedef DirectX::XMUINT4 uint4;

#define DEFINE_DELEGATE_NO_PARAM(Name) \
class Name\
{\
public:\
void Bind(void (*f)()){deleg.insert(f);}\
void UnBind(void (*f)()){deleg.erase(f);}\
void Broadcast(){if(!deleg.empty())for(auto f : deleg){f();}}\
bool IsBound(void (*f)()){auto it = deleg.find(f);return it!= deleg.end();}\
private:\
std::set<void (*)()> deleg;\
}


#define DEFINE_DELEGATE_ONE_PARAM(Name, ParamType0) \
class Name\
{\
public:\
void Bind(void (*f)(ParamType0)){deleg.insert(f);}\
void UnBind(void (*f)(ParamType0)){deleg.erase(f);}\
void Broadcast(ParamType0 p0){if(!deleg.empty())for(auto f : deleg){f(p0);}}\
bool IsBound(void (*f)(ParamType0)){auto it = deleg.find(f);return it!= deleg.end();}\
private:\
std::set<void (*)(ParamType0)> deleg;\
}

#define DEFINE_DELEGATE_TWO_PARAM(Name, ParamType0, ParamType1) \
class Name\
{\
public:\
void Bind(void (*f)(ParamType0, ParamType1)){deleg.insert(f);}\
void UnBind(void (*f)(ParamType0, ParamType1)){deleg.erase(f);}\
void Broadcast(ParamType0 p0, ParamType1 p1){if(!deleg.empty())for(auto f : deleg){f(p0, p1);}}\
bool IsBound(void (*f)(ParamType0, ParamType1)){auto it = deleg.find(f);return it!= deleg.end();}\
private:\
std::set<void (*)(ParamType0, ParamType1)> deleg;\
}

