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

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.Position = mul(float4(vin.Position, 1.f), WVP);
	vout.WorldPosition = vin.Position;
	vout.WorldNormal = vin.Normal;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float3 PixelToCamera = normalize(CameraPosition - pin.WorldPosition);
	return float4(dot(PixelToCamera, pin.WorldNormal).xxx, 1);
}