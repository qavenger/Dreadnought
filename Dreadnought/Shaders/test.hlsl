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
	float4x4 WVP;
	float3 CameraPosition;
};

cbuffer TT : register(b1)
{
	float Intensity;
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.Position = mul(float4(vin.Position, 1.f), WVP);
	vout.WorldPosition = vin.Position;
	vout.WorldNormal = vin.Normal.xyz;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float3 PixelToCamera = normalize(CameraPosition - pin.WorldPosition);
	float3 L = normalize(float3(.18, .3, 1));
	return float4(dot(L, pin.WorldNormal).xxx * Intensity, 1 + PixelToCamera.x);
}