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

	vout.Position = mul(WVP, float4(vin.Position, 1.f));
	vout.WorldPosition = vin.Position;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float3 L = normalize(float3(.2, .3, 1));
	//return float4(1,1,1,Color.r);
	float3 x = ddx(pin.WorldPosition);
	float3 y = ddy(pin.WorldPosition);
	float3 n = normalize(cross(x, y));

	float nol = saturate( dot(n, L) );

	return float4(nol, nol, nol, Color.r);
}