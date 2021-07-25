#pragma once

class Mesh
{
public:
	void Build(IRHIDevice* Device);
	void Draw(IRHIDevice* Device);
	void Destroy();

public:
	void MoveForward();
	void MoveUp();
	void MoveDown();
	void Stop();
	void MoveBackward();
	void MoveRight();
	void MoveLeft();
	RHIVertexBuffer*           VertexBuffer;
	RHIIndexBuffer*            IndexBuffer;
	EPrimitiveTopology         PrimitiveTopology;
	RHIShader*                 VertexShader;
	RHIShader*                 PixelShader;
	RHIPipelineStateObject*    PSO;
	// tmp
	float distance = 2;
	float height = 0;
	float heightDirection = 0;
	float direction = 0;
	float angle = 0;
	float angleSpeed = 0;
	// tmp - end
	std::string                ObjectName;
};

