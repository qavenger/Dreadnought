#pragma once
class Mesh
{
public:
	void Build(IRHIDevice* Device) const;
	void Draw(IRHIDevice* Device) const;


public:
	IVertexBuffer*        VertexBuffer;
	IIndexBuffer*         IndexBuffer;
};

