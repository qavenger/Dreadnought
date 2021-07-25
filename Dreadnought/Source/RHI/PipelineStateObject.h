#pragma once

struct BlendState
{
	EBlendMode                ColorSrcBlendMode[MAX_RENDER_TARGET];
	EBlendMode                ColorDestBlendMode[MAX_RENDER_TARGET];
	EBlendOperator            ColorBlendOperator[MAX_RENDER_TARGET];
	EBlendMode                AlphaSrcBlendMode[MAX_RENDER_TARGET];
	EBlendMode                AlphaDestBlendMode[MAX_RENDER_TARGET];
	EBlendOperator            AlphaBlendOperator[MAX_RENDER_TARGET];
	EWriteMask                WriteMask[MAX_RENDER_TARGET];

	bool IsRenderTargetEnableBlend(uint32 Index) const;
};

class RHIShader;
class RHIConstantBuffer;
class RHIRenderTarget;
class RHIPipelineStateObject
{
public:
	RHIPipelineStateObject();
	virtual ~RHIPipelineStateObject();

	virtual void Init() = 0;

public:
	BlendState                             State;
	EPrimitiveTopology                     GsHsPrimitiveTopology;
	EPrimitiveTopology                     PrimitiveTopology;
	ECullMode                              CullMode;
	EFillMode                              FillMode;
	RHIShader*                             VertexShader;
	RHIShader*                             PixelShader;
	RHIShader*                             GeometryShader;
	RHIShader*                             ComputerShader;
	std::vector<RHIConstantBuffer*>        ConstantBuffers;
	EDepthFunction                         DepthFunction;
	bool                                   EnableDepthTest;
	bool                                   EnableDepthWrite;
	bool                                   EnableStencilTest;

	uint32                                 NumRenderTarget;
	bool                                   IsBackbuffer;
	RHIRenderTarget*                       SceneColorRenderTarget[MAX_RENDER_TARGET];
	RHIRenderTarget*                       DepthStencilRenderTarget;
};