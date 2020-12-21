#include "Common.hlsl"

//= Vertex Shader ==========================================================

// Constant Buffers
cbuffer BufferObject : register(b1)
{
	matrix world;
	matrix wvp;
}

Pixel_PosTexNorTan mainVS(Vertex_PosTexNorTan input)
{
	Pixel_PosTexNorTan output = (Pixel_PosTexNorTan)0;

	output.position.w	= 0.0f;
	output.position		= mul(input.position, wvp);

	output.uv			= input.uv;
	output.normal		= mul(input.normal, (float3x3)world).xyz;
	output.tangent		= mul(input.tangent, (float3x3)world).xyz;

	return output;
}

//= Pixel Shader ===========================================================

// Textures
Texture2D texAlbedo : register(t0);

// Samplers
SamplerState samplerAniso : register(s0);

cbuffer BufferMatrial : register(b2)
{
	float4 materialAlbedoColor;
	
	float2 materialTilling;
	float2 materialOffset;

	float materialSpecExp;
	float materialSpecIntensity;
	float2 padding;
}

struct Output_Taget
{
	float4 albedo	: SV_TARGET0;
	float4 normal	: SV_TARGET1;
	float4 material : SV_TARGET2;
};

Output_Taget mainPS(Pixel_PosTexNorTan input)
{
	Output_Taget output;
	output.albedo	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.normal	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.material = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 albedo	= materialAlbedoColor;
	float2 texCoord = float2(input.uv.x * materialTilling.x + materialOffset.x, input.uv.y * materialTilling.y + materialOffset.y);
	float3 normal	= normalize(input.normal);

#if ALBEDO
	float4 albedoSample = texAlbedo.Sample(samplerAniso, texCoord);
	albedo *= albedoSample;
#endif

	output.albedo	= albedo;
	output.normal	= float4(normal * 0.5f + 0.5f, 0.0f);
	output.material = float4(materialSpecExp, materialSpecIntensity, 0.0f, 0.0f);

	return output;
}