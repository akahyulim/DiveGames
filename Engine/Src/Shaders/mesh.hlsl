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

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.texCoord = input.texCoord;

	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)world);
	output.tangent = normalize(output.tangent);

	return output;
}

// 추후 common_sampler로 이동
SamplerState linearSampler;

float4 mainPS(Pixel_Input input) : SV_TARGET
{
	float4 texColor = material_Albedo.Sample(linearSampler, input.texCoord);
	return texColor * materialColor;
}