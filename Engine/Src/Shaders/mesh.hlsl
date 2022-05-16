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

// 추후 common_sampler로 이동
SamplerState linearSampler;

float4 CalcuDirectionalLight(float3 normal)
{
	float4 lightColor = float4(g_light_color, 1.0f);
	float3 lightDir = g_dir_light_dir;

	// diffuse
	float lightIntensity = saturate(dot(normal, -lightDir));
	lightColor = saturate(lightColor * lightIntensity);

	// specular
	// 카메라 위치가 필요하다.

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
		// 일단 현재 sponza의 normal이 없다고 나온다.
		// 그리고 샘플링도 이렇게 하는 게 아닐 것이다.
		normal *= material_Normal.Sample(linearSampler, input.texCoord);
	}

	float4 color = albedo;

	// 곱하는 것인지, 더하는 것인지... 테스트를 해봐도 모르겠다.
	// rastertek은 곱했고, 책은 더했다...
	color *= CalcuDirectionalLight(normal);

	output.albedo = color;

	return output;
}