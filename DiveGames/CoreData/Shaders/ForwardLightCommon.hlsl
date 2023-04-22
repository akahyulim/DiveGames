#include "ConstantBuffers.hlsl"

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

PixelInput MainVS(VertexInput input)
{
	PixelInput output;

	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.worldPos = mul(input.position, worldMatrix).xyz;

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	return output;
}