#pragma once

class IMeshExporter
{
public:
	static void ExportMesh(const std::string& Filename, std::vector<uint16>& Indices,
		std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2);
};