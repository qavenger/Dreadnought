struct VertexIn
{
	float3 Position  : POSITION;
	//	float4 Color : COLOR;
};

struct VertexOut
{
	float4 Position  : SV_POSITION;
	float3 WorldPosition : COLOR;
	//	float4 Color : COLOR;
};

cbuffer OutColor : register(b0)
{
	float4x4 WVP;
	float3 Color;
};

cbuffer CC : register(b1)
{
	float b;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.Position = mul(float4(vin.Position, 1.f), WVP);
	vout.WorldPosition = vin.Position;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return float4(pin.WorldPosition, Color.r);
}