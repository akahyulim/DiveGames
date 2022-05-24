#include "common.hlsl"

struct Pixel_Input
{
	float4 position : SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
};

Pixel_Input mainVS(Vertex_PosTexNorTan input)
{
	Pixel_Input output;

	input.position.w = 1.0f;

	output.position = mul(input.position, g_World);
	output.position = mul(output.position, g_View);
	output.position = mul(output.position, g_Proj);

	output.texCoord = input.texCoord;

	output.normal = mul(input.normal, (float3x3)g_World);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)g_World);
	output.tangent = normalize(output.tangent);

	return output;
}

struct Pixel_GBuffer_Out
{
	float4 Albedo	: SV_TARGET0;
	float4 Normal	: SV_TARGET1;
	float4 Material : SV_TARGET2;
};

Pixel_GBuffer_Out mainPS(Pixel_Input input)
{
	Pixel_GBuffer_Out output;

	output.Albedo = float4(1.0f, 0.0f, 0.0f, 1.0f);
	output.Normal = float4(input.normal.xyz, 1.0f);
	output.Material = float4(0.0f, 0.0f, 1.0f, 1.0f);

	return output;
}