#include "stdafx.h"
#include "DeferredRenderer.h"
#include "Mesh/Mesh.h"
#include "Mesh/InternalMesh.h"

Mesh SubMesh;
void DeferredRenderer::OnInit() 
{
	std::vector<float3> Positions, Normals;
	std::vector<float2> UVs1, UVs2;
	std::vector<uint16> Index;
	InternalMesh::GenerateCubeInternalMesh(Index, Positions, Normals, UVs1, UVs2);
	std::vector<ETextureFormat> Layout;
	Layout.push_back(ETextureFormat::TF_R32G32B32);
	std::vector<float> Vertex;
	Vertex.resize(Positions.size() * sizeof(float3));
	memcpy(&Vertex[0], &Positions[0], Vertex.size());
	SubMesh.IndexBuffer = gDevice->CreateIndexBuffer();
	SubMesh.VertexBuffer = gDevice->CreateVertexBuffer();
	SubMesh.IndexBuffer->SetData(Index);
	SubMesh.VertexBuffer->SetData(Positions.size(), Vertex, Layout);
	SubMesh.Build(gDevice);
}

void DeferredRenderer::OnDestroy()
{
	ImGui_ImplDX12_Shutdown();
}

void DeferredRenderer::OnPostTick(float dt)
{
	/*ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();*/
//	DeviceResources->BeginFrame();

	/*if (gEngine->GetShowConsole())
	{
		Console::GetInstance().Draw("Console", &gEngine->GetShowConsole());
	}*/

//	DeviceResources->PrepareGUI();
//	ImGui::Render();
	SubMesh.Draw(gDevice);
	float color[] = { (float)sin(Engine::GetGameTime()),0,0,1 };
	/*gDevice->ClearRenderTargetView(color);

	D3D12_CPU_DESCRIPTOR_HANDLE handle[] = { gDevice->GetRenderTargetView() };
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DeviceResources->GetDepthStencilView();
	DeviceResources->GraphicsCommandList->OMSetRenderTargets(ARRAYSIZE(handle),
		handle,
		TRUE,
		&dsvHandle
	);*/

	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DeviceResources->GraphicsCommandList.Get());
	//DeviceResources->EndFrame();
}