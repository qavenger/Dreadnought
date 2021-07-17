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

class RHIPipelineStateObject
{
public:
	RHIPipelineStateObject();
	virtual ~RHIPipelineStateObject() {}

public:
	BlendState                State;
	EPrimitiveTopology        GsHsPrimitiveTopology;
	ECullMode                 CullMode;
	EFillMode                 FillMode;
	RHIShader*                  VertexShader;
	RHIShader*                  PixelShader;
	RHIShader*                  GeometryShader;
	RHIShader*                  ComputerShader;
	EDepthFunction            DepthFunction;
	bool                      EnableDepthTest;
	bool                      EnableDepthWrite;
	bool                      EnableStencilTest;

	uint32                    NumRenderTarget;
	bool                      IsBackbuffer;
	RHIRenderTarget*            SceneColorRenderTarget[MAX_RENDER_TARGET];
	RHIRenderTarget*            DepthStencilRenderTarget;
};