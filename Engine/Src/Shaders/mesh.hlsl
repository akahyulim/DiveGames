#include "common.hlsl"

struct Pixel_Input
{
	float4 position : SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 worldPos : TEXCOORD1;
};

struct Pixel_Output
{
	float4 albedo : SV_TARGET0;
};

Pixel_Input mainVS(Vertex_PosTexNorTan input)
{
	Pixel_Input output;

	input.position.w = 1.0f;

	output.position = mul(input.position, g_World);
	output.position = mul(output.position, g_View);
	output.position = mul(output.position, g_Proj);

	output.texCoord = input.texCoord;

	output.normal = mul(input.normal, (float3x3)g_World);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)g_World);
	output.tangent = normalize(output.tangent);

	output.worldPos = mul(input.position, g_World);

	return output;
}

// ���� common_sampler�� �̵�
SamplerState linearSampler;

float3 CalcuDirectionalLight(float3 pos, float3 normal)
{
	float3 lightDir = -g_DirLightDir.xyz;
	float3 eyePos = g_EyePos.xyz;

	// phong diffuse
	float NDotL = dot(normal, lightDir);
	float3 lightColor = g_LightColor.rgb * saturate(NDotL);

	// blinn specular
	// �����ߴ� �ͺ��� ���� ���� �ʴ´�.
	float3 toEye = normalize(eyePos - pos);
	float3 halfWay = normalize(toEye + lightDir);
	float NDotH = saturate(dot(normal, halfWay));
	lightColor += g_LightColor.rgb * pow(NDotH, 250.0f) * 0.25f;

	return lightColor;
}

float3 CalcuPointLight(float3 worldPos, float3 normal)
{
	// world worldPos�� ����ؾ� �Ѵ�...
	float3 toLight = g_LightPos.xyz - worldPos;
	float distance = length(toLight);
	float3 toEye = g_EyePos.xyz - worldPos;

	// phong diffuse
	toLight /= distance;
	float NDotL = saturate(dot(toLight, normal));
	float3 lightColor = g_LightColor.rgb * NDotL;

	// blinn specular
	toEye = normalize(toEye);
	float3 halfWay = normalize(toEye + toLight);
	float NDotH = saturate(dot(normal, halfWay));
	lightColor += g_LightColor.rgb * pow(NDotH, 250.0f) * 0.25f;

	// attenuation
	float distToLightNorm = 1.0 - saturate(distance * g_LightRange);
	float attn = distToLightNorm * distToLightNorm;
	lightColor *= attn;

	return lightColor;
}

Pixel_Output mainPS(Pixel_Input input)
{
	Pixel_Output output;

	// albedo
	float4 albedo = g_MatColor;
	if (HasAlbedoTexture())
	{
		// map�� �ִٸ� material color�� �����ϴ� ���� �°���?
		albedo = material_Albedo.Sample(linearSampler, input.texCoord);
		albedo.w = 1.0f;
	}

	// normal
	float3 normal = input.normal;
	if (HasNormalTexture())
	{
		// �ϴ� ���� sponza�� normal�� ���ٰ� ���´�.
		// �׸��� ���ø��� �̷��� �ϴ� �� �ƴ� ���̴�.
	//	normal *= material_Normal.Sample(linearSampler, input.texCoord);
	}

	float3 lightColor = 0;
	
	if (IsDirectionalLight())
		lightColor = CalcuDirectionalLight(input.position, normal);
	if (IsPointLight())
		lightColor = CalcuPointLight(input.worldPos, normal);

	output.albedo = float4(lightColor, 1.0f) * albedo;

	return output;
}