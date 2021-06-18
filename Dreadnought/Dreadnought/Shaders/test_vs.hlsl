struct VS_IN
{
	float3 Position : Position;
	float2 UV		: Texcoord0;
	float3 Normal	: Normal;
	float3 Tangent	: Tangent;
	float3 VertexColor : Color0;
};


float4 main(VS_IN in_data) : SV_Position
{
	return float4(in_data.Position, 1);
}