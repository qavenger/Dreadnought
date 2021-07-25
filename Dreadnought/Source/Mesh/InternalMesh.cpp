#include <stdafx.h>
#include "InternalMesh.h"
#include <MathUtil.h>

void InternalMesh::GenerateCubeInternalMesh(std::vector<uint16>& Indices, std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2)
{
	const int NumIndices = 36;
	const int NumVertices = 24;
	Indices.resize(NumIndices);
	Vertices.resize(NumVertices);
	Normals.resize(NumVertices);
	UVs1.resize(NumVertices);
	UVs2.resize(NumVertices);

	// front face
	Vertices[0] = float3(-1.f, -1.f, 1.f);
	Normals[0] = float3(0.f, 0.f, 1.f);
	UVs1[0] = float2(0.f, 0.f);
	UVs2[0] = float2(0.f, 0.5f);

	Vertices[1] = float3(1.f, -1.f, 1.f);
	Normals[1] = float3(0.f, 0.f, 1.f);
	UVs1[1] = float2(1.f, 0.f);
	UVs2[1] = float2(0.33333f, 0.5f);

	Vertices[2] = float3(1.f, 1.f, 1.f);
	Normals[2] = float3(0.f, 0.f, 1.f);
	UVs1[2] = float2(1.f, 1.f);
	UVs2[2] = float2(0.33333f, 0.f);

	Vertices[3] = float3(-1.f, 1.f, 1.f);
	Normals[3] = float3(0.f, 0.f, 1.f);
	UVs1[3] = float2(0.f, 1.f);
	UVs2[3] = float2(0.f, 0.f);

	// triangle 0
	Indices[0] = 0; Indices[1] = 1; Indices[2] = 2;
	// triangle 1
	Indices[3] = 0; Indices[4] = 2; Indices[5] = 3;

	// back
	Vertices[4] = float3(-1.f, -1.f, -1.f);
	Normals[4] = float3(0.f, 0.f, -1.f);
	UVs1[4] = float2(0.f, 0.f);
	UVs2[4] = float2(0.33333f, 0.5f);

	Vertices[5] = float3(1.f, -1.f, -1.f);
	Normals[5] = float3(0.f, 0.f, -1.f);
	UVs1[5] = float2(1.f, 0.f);
	UVs2[5] = float2(0.66666f, 0.5f);

	Vertices[6] = float3(1.f, 1.f, -1.f);
	Normals[6] = float3(0.f, 0.f, -1.f);
	UVs1[6] = float2(1.f, 1.f);
	UVs2[6] = float2(0.66666f, 0.f);

	Vertices[7] = float3(-1.f, 1.f, -1.f);
	Normals[7] = float3(0.f, 0.f, -1.f);
	UVs1[7] = float2(0.f, 1.f);
	UVs2[7] = float2(0.33333f, 0.f);

	// triangle 2
	Indices[6] = 5; Indices[7] = 4; Indices[8] = 7;
	// triangle 3
	Indices[9] = 5; Indices[10] = 7; Indices[11] = 6;

	// left
	Vertices[8] = float3(-1.f, -1.f, 1.f);
	Normals[8] = float3(-1.f, 0.f, 0.f);
	UVs1[8] = float2(0.f, 0.f);
	UVs2[8] = float2(0.66666f, 0.5f);

	Vertices[9] = float3(-1.f, -1.f, -1.f);
	Normals[9] = float3(-1.f, 0.f, 0.f);
	UVs1[9] = float2(1.f, 0.f);
	UVs2[9] = float2(1.f, 0.5f);

	Vertices[10] = float3(-1.f, 1.f, -1.f);
	Normals[10] = float3(-1.f, 0.f, 0.f);
	UVs1[10] = float2(1.f, 1.f);
	UVs2[10] = float2(1.f, 0.f);

	Vertices[11] = float3(-1.f, 1.f, 1.f);
	Normals[11] = float3(-1.f, 0.f, 0.f);
	UVs1[11] = float2(0.f, 1.f);
	UVs2[11] = float2(0.66666f, 0.f);

	// triangle 4
	Indices[12] = 8; Indices[13] = 10; Indices[14] = 9;
	// triangle 5
	Indices[15] = 8; Indices[16] = 11; Indices[17] = 10;

	// right
	Vertices[12] = float3(1.f, -1.f, 1.f);
	Normals[12] = float3(1.f, 0.f, 0.f);
	UVs1[12] = float2(0.f, 0.f);
	UVs2[12] = float2(0.f, 1.f);

	Vertices[13] = float3(1.f, -1.f, -1.f);
	Normals[13] = float3(1.f, 0.f, 0.f);
	UVs1[13] = float2(1.f, 0.f);
	UVs2[13] = float2(0.33333f, 1.f);

	Vertices[14] = float3(1.f, 1.f, -1.f);
	Normals[14] = float3(1.f, 0.f, 0.f);
	UVs1[14] = float2(1.f, 1.f);
	UVs2[14] = float2(0.33333f, 0.5f);

	Vertices[15] = float3(1.f, 1.f, 1.f);
	Normals[15] = float3(1.f, 0.f, 0.f);
	UVs1[15] = float2(0.f, 1.f);
	UVs2[15] = float2(0.f, 0.5f);

	// triangle 6
	Indices[18] = 12; Indices[19] = 13; Indices[20] = 14;
	// triangle 7
	Indices[21] = 12; Indices[22] = 14; Indices[23] = 15;

	// top
	Vertices[16] = float3(-1.f, 1.f, 1.f);
	Normals[16] = float3(0.f, 1.f, 0.f);
	UVs1[16] = float2(0.f, 0.f);
	UVs2[16] = float2(0.33333f, 1.f);

	Vertices[17] = float3(1.f, 1.f, 1.f);
	Normals[17] = float3(0.f, 1.f, 0.f);
	UVs1[17] = float2(1.f, 0.f);
	UVs2[17] = float2(0.66666f, 1.f);

	Vertices[18] = float3(1.f, 1.f, -1.f);
	Normals[18] = float3(0.f, 1.f, 0.f);
	UVs1[18] = float2(1.f, 1.f);
	UVs2[18] = float2(0.66666f, 0.5f);

	Vertices[19] = float3(-1.f, 1.f, -1.f);
	Normals[19] = float3(0.f, 1.f, 0.f);
	UVs1[19] = float2(0.f, 1.f);
	UVs2[19] = float2(0.33333f, 0.5f);

	// triangle 8
	Indices[24] = 16; Indices[25] = 17; Indices[26] = 18;
	// triangle 9
	Indices[27] = 16; Indices[28] = 18; Indices[29] = 19;

	// bottom
	Vertices[20] = float3(-1.f, -1.f, 1.f);
	Normals[20] = float3(0.f, -1.f, 0.f);
	UVs1[20] = float2(0.f, 0.f);
	UVs2[20] = float2(0.66666f, 1.f);

	Vertices[21] = float3(1.f, -1.f, 1.f);
	Normals[21] = float3(0.f, -1.f, 0.f);
	UVs1[21] = float2(1.f, 0.f);
	UVs2[21] = float2(1.f, 1.f);

	Vertices[22] = float3(1.f, -1.f, -1.f);
	Normals[22] = float3(0.f, -1.f, 0.f);
	UVs1[22] = float2(1.f, 1.f);
	UVs2[22] = float2(1.f, 0.5f);

	Vertices[23] = float3(-1.f, -1.f, -1.f);
	Normals[23] = float3(0.f, -1.f, 0.f);
	UVs1[23] = float2(0.f, 1.f);
	UVs2[23] = float2(0.66666f, 0.5f);

	// triangle 10
	Indices[30] = 20; Indices[31] = 23; Indices[32] = 22;
	// triangle 11
	Indices[33] = 20; Indices[34] = 22; Indices[35] = 21;
}

void InternalMesh::GenerateSphereInternalMesh(std::vector<uint16>& Indices, std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2)
{
	const int NumSlices = 11;
	const int NumVerticesPerSlices = 64;
	const int TotalVertices = NumSlices * NumVerticesPerSlices + 2; // 2 means two pole
	const int NumIndices = 6*NumVerticesPerSlices*NumSlices;

	Indices.resize(NumIndices);
	Vertices.resize(TotalVertices);
	Normals.resize(TotalVertices);
	UVs1.resize(TotalVertices);
	UVs2.resize(TotalVertices);

	// generate vertices
	Vertices[0] = float3(0.f, 0.f, 1.f);
	Normals[0] = float3(0.f, 0.f, 1.f);
	UVs1[0] = float2(0.f, 0.f); 
	UVs2[0] = UVs1[0]; // same as uvs1

	for (int Slice = 0; Slice < NumSlices; ++Slice)
	{
		float Theta = PI / (NumSlices + 1) * (Slice + 1);
		for (int VertexId = 0; VertexId < NumVerticesPerSlices; ++VertexId)
		{
			int Index = Slice * NumVerticesPerSlices + VertexId + 1;
			float Phi = TWO_PI / NumVerticesPerSlices * VertexId;
			float3 Vertex(GMath::Sin(Theta) * GMath::Cos(Phi), GMath::Sin(Theta) * GMath::Sin(Phi), GMath::Cos(Theta));
			Vertices[Index] = Vertex;
			Normals[Index] = Vertex;
			UVs1[Index] = float2(Phi / TWO_PI, Theta / PI);
			UVs2[Index] = UVs1[Index];
		}
	}

	Vertices[TotalVertices - 1] = float3(0.f, 0.f, -1.f);
	Normals[TotalVertices - 1] = float3(0.f, 0.f, -1.f);
	UVs1[TotalVertices - 1] = float2(0.f, 1.f);
	UVs2[TotalVertices - 1] = UVs1[TotalVertices - 1];

	// generate indices
	for (int Index = 0; Index < NumVerticesPerSlices; ++Index)
	{
		Indices[3 * Index + 0] = 0;
		Indices[3 * Index + 1] = Index + 1;
		Indices[3 * Index + 2] = Index == NumVerticesPerSlices - 1 ? 1 : Index + 2;
	}

	for (int Slice = 0; Slice < NumSlices - 1; ++Slice)
	{
		int FirstBaseIndex = 1 + Slice * NumVerticesPerSlices;
		int SecondBaseIndex = 1 + (Slice + 1) * NumVerticesPerSlices;
		for (int Index = 0; Index < NumVerticesPerSlices; ++Index)
		{
			int Triangle = (2 * Slice + 1) * NumVerticesPerSlices + Index * 2;
			bool Recyle = Index == NumVerticesPerSlices - 1;
			Indices[3 * Triangle + 0] = FirstBaseIndex + Index;
			Indices[3 * Triangle + 1] = SecondBaseIndex + Index;
			Indices[3 * Triangle + 2] = Recyle ? FirstBaseIndex : FirstBaseIndex + Index + 1;

			Indices[3 * Triangle + 3] = Recyle ? FirstBaseIndex : FirstBaseIndex + Index + 1;
			Indices[3 * Triangle + 4] = SecondBaseIndex + Index;
			Indices[3 * Triangle + 5] = Recyle ? SecondBaseIndex : SecondBaseIndex + Index + 1;
		}
	}

	int TriangleIndex = 2 * NumVerticesPerSlices * (NumSlices - 1) + NumVerticesPerSlices;
	for (int Index = 0; Index < NumVerticesPerSlices; ++Index)
	{
		int Triangle = TriangleIndex + Index;
		int BaseIndex = 1 + (NumSlices - 1) * NumVerticesPerSlices;
		Indices[3 * Triangle + 0] = TotalVertices - 1;
		Indices[3 * Triangle + 1] = Index == NumVerticesPerSlices - 1 ? BaseIndex : BaseIndex + Index + 1;
		Indices[3 * Triangle + 2] = BaseIndex + Index;
	}
}

void InternalMesh::GeneratePlaneInternalMesh(std::vector<uint16>& Indices, std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2)
{
	const int NumIndices = 6;
	const int NumVertices = 4;

	Indices.resize(NumIndices);
	Vertices.resize(NumVertices);
	Normals.resize(NumVertices);
	UVs1.resize(NumVertices);
	UVs2.resize(NumVertices);

	Vertices[0] = float3(-1.f, 0.f, 1.f);
	Normals[0] = float3(0.f, 1.f, 0.f);
	UVs1[0] = float2(0.f, 0.f);
	UVs2[0] = UVs1[0];

	Vertices[1] = float3(1.f, 0.f, 1.f);
	Normals[1] = float3(0.f, 1.f, 0.f);
	UVs1[1] = float2(1.f, 0.f);
	UVs2[1] = UVs1[1];

	Vertices[2] = float3(1.f, 0.f, -1.f);
	Normals[2] = float3(0.f, 1.f, 0.f);
	UVs1[2] = float2(1.f, 1.f);
	UVs2[2] = UVs1[2];

	Vertices[3] = float3(-1.f, 0.f, -1.f);
	Normals[3] = float3(0.f, 1.f, 0.f);
	UVs1[3] = float2(0.f, 1.f);
	UVs2[3] = UVs1[3];

	Indices[0] = 0; Indices[1] = 1; Indices[2] = 2;
	Indices[3] = 0; Indices[4] = 2; Indices[5] = 3;
}

void InternalMesh::GenerateConeInternalMesh(std::vector<uint16>& Indices, std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2)
{
	const int NumVerticesOfCircle = 64;
	const int NumVertices = 2 * NumVerticesOfCircle + 2; // cone and circle
	const int NumIndices = NumVerticesOfCircle * 6;

	Indices.resize(NumIndices);
	Vertices.resize(NumVertices);
	Normals.resize(NumVertices);
	UVs1.resize(NumVertices);
	UVs2.resize(NumVertices);

	const float TWOPI = 2.f * PI;
	const float Theta = PI / 6.f;
	
	Vertices[0] = float3(0.f, 0.f, 1.f / GMath::Tan(Theta));
	Normals[0] = float3(0.f, 0.f, 1.f);
	UVs1[0] = float2(0.5f, 0.5f);
	UVs2[0] = float2(0.25f, 0.25f);

	for (int VertexId = 0; VertexId < NumVerticesOfCircle; ++VertexId)
	{
		float Phi = TWOPI / NumVerticesOfCircle * VertexId;
		float CosPhi = GMath::Cos(Phi);
		float SinPhi = GMath::Sin(Phi);
		Vertices[VertexId + 1] = float3(CosPhi, SinPhi, 0.f);
		Normals[VertexId + 1] = Vertices[VertexId + 1];
		UVs1[VertexId + 1] = float2(CosPhi * 0.5f + 0.5f, SinPhi * 0.5f + 0.5f);
		UVs2[VertexId + 1] = float2(CosPhi * 0.25f + 0.25f, SinPhi * 0.25f + 0.25f);
	}

	for (int VertexId = 0; VertexId < NumVerticesOfCircle; ++VertexId)
	{
		float Phi = TWOPI / NumVerticesOfCircle * VertexId;
		float CosPhi = GMath::Cos(Phi);
		float SinPhi = GMath::Sin(Phi);
		Vertices[VertexId + NumVerticesOfCircle + 1] = float3(CosPhi, SinPhi, 0.f);
		Normals[VertexId + NumVerticesOfCircle + 1] = float3(0.f, 0.f, -1.f);
		UVs1[VertexId + NumVerticesOfCircle + 1] = float2(CosPhi * 0.5f + 0.5f, SinPhi * 0.5f + 0.5f);
		UVs2[VertexId + NumVerticesOfCircle + 1] = float2(CosPhi * 0.25f + 0.75f, SinPhi * 0.25f + 0.75f);
	}

	Vertices[NumVertices - 1] = float3(0.f, 0.f, 0.f);
	Normals[NumVertices - 1] = float3(0.f, 0.f, -1.f);
	UVs1[NumVertices - 1] = float2(0.5f, 0.5f);
	UVs2[NumVertices - 1] = float2(0.75f, 0.75f);

	for (int TriangleId = 0; TriangleId < NumVerticesOfCircle; ++TriangleId)
	{
		Indices[3 * TriangleId + 0] = 0;
		Indices[3 * TriangleId + 1] = TriangleId + 1;
		Indices[3 * TriangleId + 2] = TriangleId == NumVerticesOfCircle - 1 ? 1 : TriangleId + 2;
	}

	for (int TriangleId = 0; TriangleId < NumVerticesOfCircle; ++TriangleId)
	{
		int Base = TriangleId + NumVerticesOfCircle;
		Indices[3 * Base + 0] = NumVertices - 1;
		Indices[3 * Base + 1] = 2 * NumVerticesOfCircle - TriangleId;
		Indices[3 * Base + 2] = TriangleId == NumVerticesOfCircle - 1 ? 2 * NumVerticesOfCircle : 2 * NumVerticesOfCircle - TriangleId - 1;
	}
}

void InternalMesh::GenerateCylinderInternalMesh(std::vector<uint16>& Indices, std::vector<float3>& Vertices, std::vector<float3>& Normals, std::vector<float2>& UVs1, std::vector<float2>& UVs2)
{
	const int NumVerticesOfCircle = 64;
	const int NumVertices = 4 * NumVerticesOfCircle + 2;
	const int NumIndices = 12 * NumVertices;

	Indices.resize(NumIndices);
	Vertices.resize(NumVertices);
	Normals.resize(NumVertices);
	UVs1.resize(NumVertices);
	UVs2.resize(NumVertices);

	Vertices[0] =	float3(0.f, 0.f, 1.f);
	Normals[0] =	float3(0.f, 0.f, 1.f);
	UVs1[0] = float2(0.5f, 0.5f);
	UVs2[0] = float2(0.f, 0.f);

	for (int VertexId = 0; VertexId < NumVerticesOfCircle; ++VertexId)
	{
		float Phi = TWO_PI / NumVerticesOfCircle * VertexId;
		float CosPhi = GMath::Cos(Phi);
		float SinPhi = GMath::Sin(Phi);
		Vertices[VertexId + 1] = float3(CosPhi, SinPhi, 1.f);
		Normals[VertexId + 1] = float3(0.f, 0.f, 1.f);
		UVs1[VertexId + 1] = float2(CosPhi * 0.5f + 0.5f, SinPhi * 0.5f + 0.5f);
		UVs2[VertexId + 1] = float2(CosPhi * 0.25f + 0.25f, SinPhi * 0.25f + 0.25f);
	}

	for (int VertexId = 0; VertexId < NumVerticesOfCircle; ++VertexId)
	{
		float Phi = TWO_PI / NumVerticesOfCircle * VertexId;
		float CosPhi = GMath::Cos(Phi);
		float SinPhi = GMath::Sin(Phi);
		int Base = NumVerticesOfCircle + VertexId;
		Vertices[Base + 1] = float3(CosPhi, SinPhi, 1.f);
		Normals[Base + 1] = float3(CosPhi, SinPhi, 0.f);
		UVs1[Base + 1] = float2(Phi / TWO_PI, 1.f);
		UVs2[Base + 1] = float2(Phi / TWO_PI, 1.f);
	}

	for (int VertexId = 0; VertexId < NumVerticesOfCircle; ++VertexId)
	{
		float Phi = TWO_PI / NumVerticesOfCircle * VertexId;
		float CosPhi = GMath::Cos(Phi);
		float SinPhi = GMath::Sin(Phi);
		int Base = 2 * NumVerticesOfCircle + VertexId;
		Vertices[Base + 1] = float3(CosPhi, SinPhi, -1.f);
		Normals[Base + 1] = float3(CosPhi, SinPhi, 0.f);
		UVs1[Base + 1] = float2(Phi / TWO_PI, 0.f);
		UVs2[Base + 1] = float2(Phi / TWO_PI, 0.5f);
	}

	for (int VertexId = 0; VertexId < NumVerticesOfCircle; ++VertexId)
	{
		float Phi = TWO_PI / NumVerticesOfCircle * VertexId;
		float CosPhi = GMath::Cos(Phi);
		float SinPhi = GMath::Sin(Phi);
		int Base = 3 * NumVerticesOfCircle + VertexId;
		Vertices[Base + 1] = float3(CosPhi, SinPhi, -1.f);
		Normals[Base + 1] = float3(0.f, 0.f, -1.f);
		UVs1[Base + 1] = float2(CosPhi * 0.5f + 0.5f, SinPhi * 0.5f + 0.5f);
		UVs2[Base + 1] = float2(CosPhi * 0.25f + 0.75f, SinPhi * 0.25f + 0.25f);
	}

	Vertices[NumVertices - 1] = float3(0.f, 0.f, -1.f);
	Normals[NumVertices - 1] = float3(0.f, 0.f, -1.f);
	UVs1[NumVertices - 1] = float2(0.5f, 0.5f);
	UVs2[NumVertices - 1] = float2(0.f, 0.f);

	for (int TriangleId = 0; TriangleId < NumVerticesOfCircle; ++TriangleId)
	{
		Indices[3 * TriangleId + 0] = 0;
		Indices[3 * TriangleId + 1] = TriangleId + 1;
		Indices[3 * TriangleId + 2] = TriangleId == NumVerticesOfCircle - 1 ? 1 : TriangleId + 2;
	}

	for (int TriangleId = 0; TriangleId < NumVerticesOfCircle; ++TriangleId)
	{
		int Base = NumVerticesOfCircle + 2 * TriangleId;
		int SecondRowBase = 2 * NumVerticesOfCircle;
		bool Recyle = TriangleId == NumVerticesOfCircle - 1;
		Indices[3 * Base + 0] = NumVerticesOfCircle + TriangleId;
		Indices[3 * Base + 1] = SecondRowBase + TriangleId;
		Indices[3 * Base + 2] = Recyle ? NumVerticesOfCircle : NumVerticesOfCircle + TriangleId + 1;

		Indices[3 * Base + 3] = Recyle ? NumVerticesOfCircle : NumVerticesOfCircle + TriangleId + 1;
		Indices[3 * Base + 4] = SecondRowBase + TriangleId;
		Indices[3 * Base + 5] = Recyle ? 2 * NumVerticesOfCircle : SecondRowBase + TriangleId + 1;
	}

	for (int TriangleId = 0; TriangleId < NumVerticesOfCircle; ++TriangleId)
	{
		int Base = 3 * NumVerticesOfCircle + TriangleId;
		Indices[3 * Base + 0] = NumVertices - 1;
		Indices[3 * Base + 1] = 4 * NumVerticesOfCircle - TriangleId;
		Indices[3 * Base + 2] = TriangleId == NumVerticesOfCircle - 1 ? 4 * NumVerticesOfCircle : 4 * NumVerticesOfCircle - TriangleId - 1;
	}
}