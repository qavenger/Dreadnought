#pragma once
#include "Templates/TemplateClasses.h"
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
typedef TMulticastDelegate<> Name;

#define DEFINE_DELEGATE_ONE_PARAM(Name, ParamType0) \
typedef TMulticastDelegate<ParamType0> Name;

#define DEFINE_DELEGATE_TWO_PARAM(Name, ParamType0, ParamType1) \
typedef TMulticastDelegate<ParamType0, ParamType1> Name;

#define DEFINE_DELEGATE_THREE_PARAM(Name, ParamType0, ParamType1, ParamType2) \
typedef TMulticastDelegate<ParamType0, ParamType1, ParamType2> Name;