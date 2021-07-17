#pragma once

class Mesh
{
public:
	void Build(IRHIDevice* Device) const;
	void Draw(IRHIDevice* Device) const;
	void Destroy();

public:
	IVertexBuffer*           VertexBuffer;
	IIndexBuffer*            IndexBuffer;
	EPrimitiveTopology       PrimitiveTopology;
	IShader*                 VertexShader;
	IShader*                 PixelShader;
	IPipelineStateObject*    PSO;

	std::string              ObjectName;
};

