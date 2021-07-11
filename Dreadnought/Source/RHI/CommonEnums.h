#pragma once

enum class ECullMode
{
	CM_None,    // Twoside visible
	CM_Front,   
	CM_Back,
};

enum class EFillMode
{
	FM_Wireframe,
	FM_Solid,
};

enum class ETextureFormat
{
	TF_Unknown = 0,
	TF_R16,
	TF_R16G16,
	TF_R16G16B16A16,
	TF_R32,
	TF_R32G32,
	TF_R32G32B32,
	TF_R32G32B32A32,
	TF_R8G8B8A8,
	TF_D24S8,
	TF_D32,
	TF_D32S8,

	TF_Max,
};

enum class ETextureDimension
{
	TD_TextureBuffer,
	TD_Texture1D,
	TD_Texture2D,
	TD_Texture3D,
	TD_TextureCube,
};

enum class EBlendMode
{
	BM_Zero,
	BM_One,
	BM_SrcColor,
	BM_InverseSrcColor,
	BM_SrcAlpha,
	BM_InverseSrcAlpha,
	BM_DestColor,
	BM_InverseDestColor,
	BM_DestAlpha,
	BM_InverseDestAlpha,
};

enum class EBlendOperator
{
	BO_Add,
	BO_Sub,
	BO_ReverseSub,
	BO_Min,
	BO_Max,
};

enum class EWriteMask
{
	WM_R = 1,
	WM_G = 2,
	WM_B = 4,
	WM_A = 8,
	WM_RGB = (WM_R | WM_G | WM_B),
	WM_RGBA = (WM_RGB | WM_A),
};

enum class EPrimitiveTopology
{
	PT_Point,
	PT_Line,
	PT_Triangle,
};

enum class EDepthFunction
{
	DF_Never,
	DF_Less,
	DF_Equal,
	DF_LessEqual,
	DF_Greater,
	DF_NotEqual,
	DF_GreaterEqual,
	DF_Always,
};

enum class EShaderType
{
	ST_Vertex,
	ST_Pixel,
	ST_Geometry,
	ST_Comptuer,
};

#define MAX_RENDER_TARGET 8

