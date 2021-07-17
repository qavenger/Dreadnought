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
	Vertex.resize(Positions.size() * 3);
	for (uint32 Index = 0; Index < Positions.size(); ++Index)
	{
		Vertex[3 * Index + 0] = Positions[Index].x;
		Vertex[3 * Index + 1] = Positions[Index].y;
		Vertex[3 * Index + 2] = Positions[Index].z;
	}
	SubMesh.IndexBuffer = gDevice->CreateIndexBuffer();
	SubMesh.VertexBuffer = gDevice->CreateVertexBuffer();
	SubMesh.IndexBuffer->SetData(Index);
	SubMesh.VertexBuffer->SetData((uint32)Positions.size(), Vertex, Layout);
	ShaderConstructDesc VertexShaderDesc, PixelShaderDesc;
	VertexShaderDesc.ShaderFile = L"D:/DIYEngine/Dreadnought/Dreadnought/Shaders/test.hlsl";
	VertexShaderDesc.ShaderEntry = "VS";
	VertexShaderDesc.ShaderType = EShaderType::ST_Vertex;
	PixelShaderDesc.ShaderFile = L"D:/DIYEngine/Dreadnought/Dreadnought/Shaders/test.hlsl";
	PixelShaderDesc.ShaderEntry = "PS";
	PixelShaderDesc.ShaderType = EShaderType::ST_Pixel;
	SubMesh.VertexShader = gDevice->CreateShader();
	SubMesh.VertexShader->SetShaderDesc(VertexShaderDesc);
	SubMesh.PixelShader = gDevice->CreateShader();
	SubMesh.PixelShader->SetShaderDesc(PixelShaderDesc);
	SubMesh.PSO = gDevice->CreatePipelineStateObject();
	SubMesh.PrimitiveTopology = EPrimitiveTopology::PT_Triangle;
	SubMesh.ObjectName = "StarSky";
	SubMesh.Build(gDevice);
}

void DeferredRenderer::OnDestroy()
{
	ImGui_ImplDX12_Shutdown();
	SubMesh.Destroy();
}

void DeferredRenderer::OnTick(float dt)
{
	SCOPE_EVENT(BasePass)
	{
		RHIRenderPassInfo Info;
		Info.ClearColor[0].ClearColorG = 1;
		gDevice->BeginRenderPass(Info);
		gDevice->SetViewport(0, 0, 1024, 720, 0, 1);
		gDevice->SetScissor(0, 0, 1024, 720);
		SubMesh.Draw(gDevice);
		gDevice->EndRenderPass();
	}
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