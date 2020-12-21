#include "Common.hlsl"

// textures
Texture2D texNormal		: register(t0);
Texture2D texMaterial	: register(t1);
Texture2D texDepth		: register(t2);

// samplers
SamplerState SamplerPointClamp			: register(s0);
SamplerComparisonState SamplerCmpDepth	: register(s1);
SamplerState SamplerLinearClamp			: register(s2);

// buffers
cbuffer BufferLight : register(b1)
{
	float3 lightPosition;
	float lightRange;
	float3 lightDirection;
	float lightOuterCone;
	float3 lightColor;
	float lightInnerCone;
};

struct LightOutput
{
	float4 diffuse	: SV_TARGET0;
	float4 specular : SV_TARGET1;
};

LightOutput mainPS(Pixel_PosTex input)
{
	LightOutput output;
	output.diffuse	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// texture sampling
	float3 sampleNormal		= texNormal.Sample(SamplerPointClamp, input.uv).xyz;
	float4 sampleMaterial	= texMaterial.Sample(SamplerPointClamp, input.uv);
	float sampleDepth		= texDepth.Sample(SamplerPointClamp, input.uv).r;

	// phong diffuse
	float3 lightDir = -normalize(lightDirection);
	float3 normal	= normalize(sampleNormal * 2.0f - 1.0f);
	float angle		= dot(normal, lightDir);
	output.diffuse	= float4(lightColor * saturate(angle), 1.0f);

	// blinn specular
	float3 worldPos = CalcuWorldPosition(sampleDepth, input.uv);
	float3 toCamera = normalize(g_cameraPos - worldPos);
	float3 HalfWay	= normalize(toCamera + lightDir);
	float NDotH		= saturate(dot(HalfWay, normal));
	output.specular = float4(lightColor * pow(NDotH, 20.0f), 1.0f);

	return output;
}
