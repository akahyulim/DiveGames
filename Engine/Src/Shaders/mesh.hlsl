#include "common.hlsl"

struct Pixel_Input
{
	float4 position : SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
};

struct Pixel_Output
{
	float4 albedo : SV_TARGET0;
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

// ���� common_sampler�� �̵�
SamplerState linearSampler;

float4 CalcuDirectionalLight(float3 normal)
{
	float4 lightColor = float4(g_light_color, 1.0f);
	float3 lightDir = g_dir_light_dir;

	// diffuse
	float lightIntensity = saturate(dot(normal, -lightDir));
	lightColor = saturate(lightColor * lightIntensity);

	// specular
	// ī�޶� ��ġ�� �ʿ��ϴ�.

	return lightColor;
}

Pixel_Output mainPS(Pixel_Input input)
{
	Pixel_Output output;

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
		// �ϴ� ���� sponza�� normal�� ���ٰ� ���´�.
		// �׸��� ���ø��� �̷��� �ϴ� �� �ƴ� ���̴�.
		normal *= material_Normal.Sample(linearSampler, input.texCoord);
	}

	float4 color = albedo;

	// ���ϴ� ������, ���ϴ� ������... �׽�Ʈ�� �غ��� �𸣰ڴ�.
	// rastertek�� ���߰�, å�� ���ߴ�...
	color *= CalcuDirectionalLight(normal);

	output.albedo = color;

	return output;
}