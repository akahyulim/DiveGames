#include "Common.hlsl"

Pixel_PosTex mainVS(Vertex_PosTex input)
{
	Pixel_PosTex output;

	output.position.w	= 1.0f;
	output.position		= mul(input.position, g_viewProjOrtho);
	output.uv			= input.uv;

	return output;
}

Texture2D texAlbedo		: register(t0);
Texture2D texDiffuse	: register(t1);
Texture2D texSpecular	: register(t2);

SamplerState samplerPoint : register(s0);

struct Output
{
	float4 composition : SV_TARGET0;
};

Output mainPS(Pixel_PosTex input)
{
	Output output;

	float4 albedo	= texAlbedo.Sample(samplerPoint, input.uv);
	float3 diffuse	= texDiffuse.Sample(samplerPoint, input.uv).xyz;
	float3 specular = texSpecular.Sample(samplerPoint, input.uv).xyz;

	output.composition = float4(albedo.xyz * (diffuse + specular), albedo.w);

	return output;
}