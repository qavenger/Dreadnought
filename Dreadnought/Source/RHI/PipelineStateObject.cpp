#include "stdafx.h"
#include "PipelineStateObject.h"

bool BlendState::IsRenderTargetEnableBlend(uint32 Index) const
{
	FatalIfFalse((Index >= 0 && Index < MAX_RENDER_TARGET), "Index must be in [0, 7]");

	if (ColorSrcBlendMode[Index] == EBlendMode::BM_One &&
		ColorDestBlendMode[Index] == EBlendMode::BM_Zero &&
		ColorBlendOperator[Index] == EBlendOperator::BO_Add &&
		AlphaSrcBlendMode[Index] == EBlendMode::BM_One &&
		AlphaDestBlendMode[Index] == EBlendMode::BM_Zero &&
		AlphaBlendOperator[Index] == EBlendOperator::BO_Add)
		return false;

	return true;
}

RHIPipelineStateObject::~RHIPipelineStateObject()
{
	if (VertexShader)
		delete VertexShader;
	if (PixelShader)
		delete PixelShader;
	if (ConstantBuffer)
		delete ConstantBuffer;
}

RHIPipelineStateObject::RHIPipelineStateObject()
{
	for (uint32 Index = 0; Index < MAX_RENDER_TARGET; ++Index)
	{
		State.ColorSrcBlendMode[Index] = EBlendMode::BM_One;
		State.ColorDestBlendMode[Index] = EBlendMode::BM_Zero;
		State.ColorBlendOperator[Index] = EBlendOperator::BO_Add;
		State.AlphaSrcBlendMode[Index] = EBlendMode::BM_One;
		State.AlphaDestBlendMode[Index] = EBlendMode::BM_Zero;
		State.AlphaBlendOperator[Index] = EBlendOperator::BO_Add;
		State.WriteMask[Index] = EWriteMask::WM_RGBA;
		SceneColorRenderTarget[Index] = nullptr;
	}
	DepthStencilRenderTarget = nullptr;

	GsHsPrimitiveTopology = EPrimitiveTopology::PT_Triangle;
	PrimitiveTopology = EPrimitiveTopology::PT_Triangle;
	CullMode = ECullMode::CM_Back;
	FillMode = EFillMode::FM_Solid;

	VertexShader = nullptr;
	PixelShader = nullptr;
	GeometryShader = nullptr;
	ComputerShader = nullptr;
	ConstantBuffer = nullptr;

	DepthFunction = EDepthFunction::DF_Less;
	EnableDepthTest = true;
	EnableDepthWrite = true;
	EnableStencilTest = false;

	NumRenderTarget = 1;
	IsBackbuffer = true;
}