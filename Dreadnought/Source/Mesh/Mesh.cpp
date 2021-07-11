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