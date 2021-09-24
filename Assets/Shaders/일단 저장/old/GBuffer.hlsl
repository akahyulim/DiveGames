#include "Common.hlsl"

//= Textures & Sampler ==========================
Texture2D texAlbedo			: register(t0);
SamplerState samplerAniso	: register(s0);
//===============================================

//= Constant Buffers ============================
cbuffer BufferMaterial : register(b1)
{
	float4 materialAlbedoColor;
	float2 materialTiling;
	float2 materialOffset;

	float materialSpecExp;
	float materialSpecIntensity;
	float2 padding;
}

cbuffer BufferObject : register(b2)
{
	matrix matWorld;
	matrix matWVP;
}
//===============================================

Pixel_PosTexNorTan mainVS(Vertex_PosTexNorTan input)
{
	Pixel_PosTexNorTan output = (Pixel_PosTexNorTan)0;

	output.position.w = 1.0f;
	output.position = mul(input.position, matWorld);
	output.position = mul(output.position, g_view);
	output.position = mul(output.position, g_proj);

	output.normal = mul(input.normal, (float3x3)matWorld);
	output.tangent = mul(input.tangent, (float3x3)matWorld);
	output.uv = input.uv;

	return output;
}

struct Out_GBuffer
{
	float4 albedo	: SV_TARGET0;
	float4 normal	: SV_TARGET1;
	float4 material : SV_TARGET2;
};

Out_GBuffer mainPS(Pixel_PosTexNorTan input)
{
	Out_GBuffer output = (Out_GBuffer)0;

	float2 texCoords = float2(input.uv.x * materialTiling.x + materialOffset.x, input.uv.y * materialTiling.y + materialOffset.y);
	float4 albedo = materialAlbedoColor;
	float4 albedoSample = texAlbedo.Sample(samplerAniso, texCoords);
	albedo *= albedoSample;

	float3 normal = normalize(input.normal);

	// GBuffer에 쓰기 - 이게 정확히 어느 space냐?
	output.albedo = albedo;
	output.normal = float4(normal * 0.5f + 0.5f, 0.0f);
	output.material = float4(materialSpecExp, materialSpecIntensity, 0.0f, 0.0f);

	return output;
}