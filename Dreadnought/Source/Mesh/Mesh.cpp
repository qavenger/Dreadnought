#include <stdafx.h>
#include "Mesh.h"

void Mesh::Build(IRHIDevice* Device) const
{
	gDevice->ResetCommandList();
	gDevice->BuildIndexBuffer(IndexBuffer);
	gDevice->BuildVertexBuffer(VertexBuffer);
	PSO->VertexShader = VertexShader;
	PSO->PixelShader = PixelShader;
	PSO->Init();
	gDevice->BuildPipelineStateObject(PSO);

	struct PerObjectData
	{
		XMFLOAT4X4 WVP;
		XMFLOAT4 Color;
	} Data;
	Data.Color = { 1,0,1,0 };
	XMFLOAT3 EyePosition = XMFLOAT3(-5, 0, -5);
	XMFLOAT3 CenterPosition = XMFLOAT3(0, 0, 0);
	XMFLOAT3 Up = XMFLOAT3(0, 1, 0);
	XMMATRIX Trans = XMMatrixLookAtLH(XMLoadFloat3(&EyePosition), XMLoadFloat3(&CenterPosition), XMLoadFloat3(&Up));
	XMStoreFloat4x4(&Data.WVP, XMMatrixTranspose(XMMatrixMultiply(Trans, XMMatrixPerspectiveFovLH(45.f, 1024 / 720.f, 0.1f, 100.f))));
	
	RHIConstantBuffer* ConstantBuffer = gDevice->CreateConstantBuffer();
	ConstantBuffer->SetElementCount(1);
	ConstantBuffer->SetElementSize(sizeof(PerObjectData));
	gDevice->BuildConstantBuffer(ConstantBuffer);
	ConstantBuffer->UpdateData(&Data, sizeof(PerObjectData), 0);
	PSO->ConstantBuffer = ConstantBuffer;
	gDevice->FlushCommandQueueSync();
}

void Mesh::Draw(IRHIDevice* Device) const
{
	SCOPE_EVENT_STR_FORMAT(Draw, "Draw_%s", ObjectName.c_str())
	{
		DrawInfo Info(IndexBuffer, VertexBuffer, PSO);
		Device->DrawElements(Info);
	}
}

void Mesh::Destroy()
{
	if (VertexBuffer)
		delete VertexBuffer;
	if (IndexBuffer)
		delete IndexBuffer;
	if (PSO)
		delete PSO;
}
