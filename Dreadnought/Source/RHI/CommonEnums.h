#pragma once

enum class ECullMode
{
	CM_None = 0,    // Twoside visible
	CM_Front,   
	CM_Back,
	
	CM_Num,
};

enum class EFillMode
{
	FM_Wireframe = 0,
	FM_Solid,

	FM_Num,
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

	TF_Num,
};

enum class ETextureDimension
{
	TD_TextureBuffer = 0,
	TD_Texture1D,
	TD_Texture2D,
	TD_Texture3D,
	TD_TextureCube,

	TD_Num,
};

enum class EBlendMode
{
	BM_Zero = 0,
	BM_One,
	BM_SrcColor,
	BM_InverseSrcColor,
	BM_SrcAlpha,
	BM_InverseSrcAlpha,
	BM_DestColor,
	BM_InverseDestColor,
	BM_DestAlpha,
	BM_InverseDestAlpha,

	BM_Num,
};

enum class EBlendOperator
{
	BO_Add = 0,
	BO_Sub,
	BO_ReverseSub,
	BO_Min,
	BO_Max,

	BO_Num
};

enum class EWriteMask
{
	WM_R = 0,
	WM_G,
	WM_B,
	WM_A,
	WM_RGB,
	WM_RGBA,

	WM_Num
};

enum class EPrimitiveTopology
{
	PT_Point = 0,
	PT_Line,
	PT_Triangle,

	PT_Num
};

enum class EDepthFunction
{
	DF_Never = 0,
	DF_Less,
	DF_Equal,
	DF_LessEqual,
	DF_Greater,
	DF_NotEqual,
	DF_GreaterEqual,
	DF_Always,

	DF_Num
};

enum class EShaderType
{
	ST_Vertex = 0,
	ST_Pixel,
	ST_Geometry,
	ST_Computer,

	ST_Num
};

enum class ETextureFilter
{
	TF_Point = 0,
	TF_Linear,
	TF_Trilinear,
	TF_Anisotropic,

	TF_Num,
};

enum class ETextureWrapMode
{
	TWM_Wrap = 0,
	TWM_Mirror,
	TWM_Clamp,

	TWM_Num,
};

