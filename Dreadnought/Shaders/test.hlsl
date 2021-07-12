struct VertexIn
{
	float3 Position  : POSITION;
	//	float4 Color : COLOR;
};

struct VertexOut
{
	float4 Position  : SV_POSITION;
	//	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.Position.xyz = vin.Position * 0.3f;
	vout.Position.w = 1.f;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return float4(1.f, 1.f, 0.f, 1.f);
}