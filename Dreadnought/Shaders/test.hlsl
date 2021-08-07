struct VertexIn
{
	float3 Position  : POSITION;
	float3 Normal : Normal;
};

struct VertexOut
{
	float4 Position  : SV_POSITION;
	float3 WorldPosition : COLOR;
	float3 WorldNormal : Normal;
};

cbuffer OutColor : register(b0)
{
	float4x4 W;
	float4x4 VP;
	float3 CameraPosition;
};

cbuffer TT : register(b1)
{
	float Intensity;
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.Position = mul(W, float4(vin.Position, 1.f));
	vout.WorldPosition = vout.Position.xyz;
	vout.Position = mul(VP, vout.Position);
	vout.WorldNormal = mul((float3x3)W ,vin.Normal.xyz);

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float3 PixelToCamera = normalize(CameraPosition - pin.WorldPosition);
	float3 L = normalize(float3(.18, .3, 1));
	return float4(dot(L, pin.WorldNormal).xxx*0.5+0.5 * Intensity, 1 + PixelToCamera.x);
}