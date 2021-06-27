#pragma once

class InternalMesh
{
public:
	static void GenerateCubeInternalMesh(std::vector<uint16>& Indices, std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2);
	static void GenerateSphereInternalMesh(std::vector<uint16>& Indices, std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2);
};