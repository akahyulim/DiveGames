#include "common.hlsl"

// textures & sampler
// ��׵� �̸��� ������ �ȵȴ�. include�����ε�
Texture2D tex_Albedo			: register(t0);
Texture2D tex_Normal			: register(t1);
Texture2D tex_Depth				: register(t2);

//SamplerState sampler_Aniso		: register(s0);
//SamplerState sampler_Linear		: register(s1);
SamplerState sampler_Test: register(s0);

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 uv		: TEXCOORD0;
};

Pixel_PosTex mainVS(Vertex_PosTex input)
{
	Pixel_PosTex output = (Pixel_PosTex)0;

	output.position.w = 1.0f;
	output.position = mul(input.position, g_viewProjOrtho);

	output.uv = input.uv;
																			 
	return output;
}

float3 CalcuDirectionalLight(float2 texCoord)
{
	// light position, diffuse color �ʿ�	=> ���� component���� constant buffer�� ���޹޾ƾ� �Ѵ�.
	float3 posLight = float3(-50.0f, 100.0f, -100.0f);
	float3 lightColor = float3(1.0f, 1.0f, 1.0f);
	float3 lightDir = float3(1.0f, -1.0f, 1.0f);

	float3 finalColor = (float3)0;

	float3 normal = tex_Normal.Sample(sampler_Test, texCoord).xyz;
	normal = normalize(normal);

	// depth���� ����� �� world position ȹ�� => ���� �߸���
	float depth = tex_Depth.Sample(sampler_Test, texCoord).r;
	float3 worldPos = GetWorldPositionFromDepth(depth, texCoord);

	// phong - diffuse
	float3 toLight = normalize(-lightDir);
	float diffuse = saturate(dot(normal, toLight));
	finalColor = lightColor * diffuse;

	// blinn - specular
	float3 toEye = normalize(g_cameraPos - worldPos);
	float3 halfWay = normalize(toEye + toLight);
	//finalColor += lightColor * pow(saturate(dot(halfWay, normal)), 20.0f);

	return saturate(finalColor);
}

float4 mainPS(Pixel_PosTex input) : SV_TARGET
{
	float4 albedo = tex_Albedo.Sample(sampler_Test, input.uv);
	float3 light = CalcuDirectionalLight(input.uv);

	return float4(light * albedo.xyz, albedo.w);
	//return albedo;
}