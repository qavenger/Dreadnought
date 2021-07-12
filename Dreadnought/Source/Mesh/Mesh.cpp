#include <stdafx.h>
#include "Mesh.h"


void Mesh::Build(IRHIDevice* Device) const
{
	gDevice->BuildIndexBuffer(IndexBuffer);
	gDevice->BuildVertexBuffer(VertexBuffer);
	gDevice->BuildShader(VertexShader);
	gDevice->BuildShader(PixelShader);
	PSO->VertexShader = VertexShader;
	PSO->PixelShader = PixelShader;
	gDevice->BuildPipelineStateObject(PSO);
}

void Mesh::Draw(IRHIDevice* Device) const
{
	gDevice->SetPipelineStateObject(PSO);
	Device->DrawIndexedInstanced(IndexBuffer, VertexBuffer);
}

void Mesh::Destroy()
{
	if(VertexBuffer)
		delete VertexBuffer;
	if (IndexBuffer)
		delete IndexBuffer;
}
