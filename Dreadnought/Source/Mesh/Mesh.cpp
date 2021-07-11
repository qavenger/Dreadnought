#include <stdafx.h>
#include "Mesh.h"


void Mesh::Build(IRHIDevice* Device) const
{
	gDevice->SetIndexBuffer(IndexBuffer);
	gDevice->SetVertexBuffer(VertexBuffer);
}

void Mesh::Draw(IRHIDevice* Device) const
{
	Device->DrawIndexedInstanced(IndexBuffer, VertexBuffer);
}

void Mesh::Destroy()
{
	if(VertexBuffer)
		delete VertexBuffer;
	if (IndexBuffer)
		delete IndexBuffer;
}
