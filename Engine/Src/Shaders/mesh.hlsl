#include "common.hlsl"

struct Pixel_Input
{
	float4 position : SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float3 worldPos : TEXCOORD1;
};

struct Pixel_GBuffer_Out
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

	// ���⿡�� xyz�� float3x3���� ����ϸ� ��ġ�� �̻�������.
	// ���� ���� normal�� tangent ��굵 �߸� �Ǿ��� �� �ִ�.
	// ������ å������ �׳� �ߴ�. �ٸ� worldPos�� �Ʒ��� �����ϴ�.
	output.worldPos = mul(input.position, g_World).xyz;

	return output;
}

// ��򰡿��� set�� ���� ���ε� ã�� ���ߴ�.
// ������ �ϴµ�...
//SamplerState linearSampler;

float3 CalcuDirectionalLight(float3 pos, float3 normal, float4 mtrlColor)
{
	float3 dirLight = 0;
	
	float3 lightDir = -g_LightDir;

	// ambient
	float ambient = 0.1f;
	dirLight = g_LightColor * ambient;

	// phong diffuse
	float diffuse = saturate(dot(normal, lightDir));
	dirLight += g_LightColor * diffuse;

	// blinn specular
	float3 toEye = normalize(g_EyePos - pos);
	float3 halfWay = normalize(toEye + lightDir);
	float specular = pow(saturate(dot(normal, halfWay)), 250.f) * 0.25f;
	dirLight += g_LightColor * specular;

	return dirLight * mtrlColor.rgb;
}

float3 CalcuPointLight(float3 pos, float3 normal, float4 mtrlColor)
{
	float3 pointLight = 0;

	float3 toLight = g_LightPos - pos;
	float lightDist = length(toLight);
	float3 toEye = normalize(g_EyePos - pos);
	
	// phong diffuse
	toLight /= lightDist;
	float diffuse = saturate(dot(normal, toLight));
	pointLight = g_LightColor * diffuse;

	// blinn specular
	float3 halfWay = normalize(toEye + toLight);
	float specular = pow(saturate(dot(normal, halfWay)), 250.f) * 0.25f;
	pointLight += g_LightColor * specular;

	// point attnuation
	float attn = 1.0f - saturate(lightDist * g_LightRangeRcp);
	attn *= attn;

	// å������ matrl�� color�� attn�� ���� �� �ٽ� diffuse * specular�� ���ߴ�.
	// �̷��� �ϸ� mtrlColor�� ���� �پ�� �� �ִ�.
	return pointLight * (mtrlColor * attn).xyz;
}

float3 CalcuSpotLight(float3 pos, float3 normal, float4 mtrlColor)
{
	float3 spotLight = 0;

	float3 toLight = g_LightPos - pos;
	float lightDist = length(toLight);
	float3 toEye = normalize(g_EyePos - pos);
	float3 lightDir = -g_LightDir;

	// phong diffuse
	toLight /= lightDist;
	float diffuse = saturate(dot(toLight, normal));
	spotLight = g_LightColor * diffuse;

	// blinn specular
	float3 halfWay = normalize(toEye + toLight);
	float specular = pow(saturate(dot(normal, halfWay)), 250.f) * 0.25f;
	spotLight += g_LightColor * specular;

	// attenuation
	float attn = 1.0 - saturate(lightDist * g_LightRangeRcp);
	attn *= attn;

	// cone attenuation
	float cosAngle = dot(lightDir, toLight);
	//float coneAttn = saturate((cosAngle - radians(cos(65.0f))) * (1 / g_SpotAngle));
	float coneAttn = saturate(cosAngle * (1 / g_SpotAngle));
	coneAttn *= coneAttn;

	// ���� å������ attn�� coneAttn�� material�� ���ߴ�.
	return spotLight * (mtrlColor * attn * coneAttn).rgb;
}

Pixel_GBuffer_Out mainPS(Pixel_Input input)
{
	Pixel_GBuffer_Out output;

	// albedo
	float4 albedo = g_MtrlColor;
	if (HasAlbedoTexture())
	{
		// map�� �ִٸ� material color�� �����ϴ� ���� �°���?
		albedo = material_Albedo.Sample(LinearSampler, input.texCoord);
		albedo.w = 1.0f;
	}

	// normal
	float3 normal = input.normal;
	if (HasNormalTexture())
	{
		// �ϴ� ���� sponza�� normal�� ���ٰ� ���´�.
		// �׸��� ���ø��� �̷��� �ϴ� �� �ƴ� ���̴�.
	//	normal *= material_Normal.Sample(LinearSampler, input.texCoord);
	}

	float3 lightColor = 0;
	
	if (IsDirectionalLight())
		lightColor = CalcuDirectionalLight(input.worldPos, normal, albedo);
	if (IsPointLight())
		lightColor = CalcuPointLight(input.worldPos, normal, albedo);
	if (IsSpotLight())
		lightColor = CalcuSpotLight(input.worldPos, normal, albedo);

	//output.albedo = float4(lightColor, 1.0f) * albedo;
	output.albedo = float4(lightColor.rgb, 1.0f);

	return output;
}