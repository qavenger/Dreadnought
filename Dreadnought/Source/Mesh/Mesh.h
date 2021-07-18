#pragma once

class Mesh
{
public:
	void Build(IRHIDevice* Device) const;
	void Draw(IRHIDevice* Device) const;
	void Destroy();

public:
	RHIVertexBuffer*           VertexBuffer;
	RHIIndexBuffer*            IndexBuffer;
	EPrimitiveTopology         PrimitiveTopology;
	RHIShader*                 VertexShader;
	RHIShader*                 PixelShader;
	RHIPipelineStateObject*    PSO;

	std::string                ObjectName;
};

