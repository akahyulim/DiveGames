#include "Common.hlsl"

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

PixelInput MainVS(Vertex_PosTexNorTan input)
{
	PixelInput output;

	input.position.w = 1.0f;
	output.position = mul(input.position, cbWorldMatrixVS);
	output.position = mul(output.position, cbViewMatrixVS);
	output.position = mul(output.position, cbProjMatrixVS);

	output.worldPos = mul(input.position, cbWorldMatrixVS).xyz;

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)cbWorldMatrixVS);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)cbWorldMatrixVS);
	output.tangent = normalize(output.tangent);

	return output;
}

struct PS_GBUFFER
{
	float4 colorSpecIntensity : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 specPower : SV_TARGET2;
};

PS_GBUFFER MainPS(PixelInput input)
{
	PS_GBUFFER output;

	float4 diffuse = DiffuseMap.Sample(DiffuseMapSampler, input.tex);
    //diffuse *= diffuse; // ¿Ö Á¦°ö?
	output.colorSpecIntensity = diffuse;

	output.normal = float4(input.normal * 0.5 + 0.5, 0.0);

	return output;
}