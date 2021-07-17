#include <stdafx.h>
#include "Mesh.h"


void Mesh::Build(IRHIDevice* Device) const
{
	gDevice->ResetCommandList();
	gDevice->BuildIndexBuffer(IndexBuffer);
	gDevice->BuildVertexBuffer(VertexBuffer);
	gDevice->BuildShader(VertexShader);
	gDevice->BuildShader(PixelShader);
	PSO->VertexShader = VertexShader;
	PSO->PixelShader = PixelShader;
	gDevice->BuildPipelineStateObject(PSO);
	gDevice->FlushCommandQueueSync();
}

void Mesh::Draw(IRHIDevice* Device) const
{
	SCOPE_EVENT_STR_FORMAT(Draw, "Draw_%s", ObjectName.c_str())
	{
		DrawInfo Info(IndexBuffer, VertexBuffer, PrimitiveTopology);
		gDevice->SetPipelineStateObject(PSO);
		Device->DrawElements(Info);
	}
}

void Mesh::Destroy()
{
	if(VertexBuffer)
		delete VertexBuffer;
	if (IndexBuffer)
		delete IndexBuffer;
}
