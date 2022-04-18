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

	output.position = mul(input.position, g_world);
	output.position = mul(output.position, g_view);
	output.position = mul(output.position, g_proj);

	output.texCoord = input.texCoord;

	output.normal = mul(input.normal, (float3x3)g_world);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)g_world);
	output.tangent = normalize(output.tangent);

	return output;
}

// 추후 common_sampler로 이동
SamplerState linearSampler;

float4 CalcuDirectionalLight(float3 normal)
{
	float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float3 lightDir = float3(1.0f, -1.0f, 1.0f);

	// diffuse
	float lightIntensity = saturate(dot(normal, -lightDir));
	lightColor = saturate(lightColor * lightIntensity);

	// specular

	return lightColor;
}

float4 mainPS(Pixel_Input input) : SV_TARGET
{
	// albedo
	float4 albedo = g_mat_color;
	if (HasAlbedoTexture())
	{
		albedo *= material_Albedo.Sample(linearSampler, input.texCoord);
		albedo.w = 1.0f;
	}

	// normal
	float3 normal = input.normal;
	if (HasNormalTexture())
	{
	//	normal *= material_Normal.Sample(linearSampler, input.texCoord);
	}

	float4 color = albedo;

	color *= CalcuDirectionalLight(normal);

	return color;
}