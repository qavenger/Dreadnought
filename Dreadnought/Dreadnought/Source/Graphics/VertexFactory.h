#pragma once
struct SimpleVertex
{
	float3 pos;
	float4 color;
};

struct Vertex
{
	float3 pos;
	float3 normal;
	float3 tangent;
	float2 uv;
};

template<int UVCount = 1>
struct VertexAddition
{
	float3 color;
	float4 uv1[UVCount];
};

struct VertexSkeletal
{
	int4 boneIndices;
	float4 boneWeight;
};

class VertexFactory
{
public:
	static D3D12_INPUT_ELEMENT_DESC* GetSkeletalMeshVertex();
};

