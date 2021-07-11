#pragma once
class Mesh
{
public:
	void Build(IRHIDevice* Device) const;
	void Draw(IRHIDevice* Device) const;
	void Destroy();

public:
	IVertexBuffer*        VertexBuffer;
	IIndexBuffer*         IndexBuffer;
};

