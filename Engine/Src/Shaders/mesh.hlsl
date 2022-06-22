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

	// 여기에서 xyz랑 float3x3으로 계산하면 위치가 이상해진다.
	// 따라서 위의 normal과 tangent 계산도 잘못 되었을 수 있다.
	// 하지만 책에서는 그냥 했다. 다만 worldPos는 아래와 동일하다.
	output.worldPos = mul(input.position, g_World).xyz;

	return output;
}

// 어딘가에서 set을 했을 것인데 찾질 못했다.
// 지워야 하는데...
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

	// 책에서는 matrl의 color에 attn을 곱한 후 다시 diffuse * specular에 곱했다.
	// 이렇게 하면 mtrlColor의 값이 줄어들 수 있다.
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

	// 역시 책에서는 attn과 coneAttn을 material에 곱했다.
	return spotLight * (mtrlColor * attn * coneAttn).rgb;
}

Pixel_GBuffer_Out mainPS(Pixel_Input input)
{
	Pixel_GBuffer_Out output;

	// albedo
	float4 albedo = g_MtrlColor;
	if (HasAlbedoTexture())
	{
		// map이 있다면 material color는 무시하는 것이 맞겠지?
		albedo = material_Albedo.Sample(LinearSampler, input.texCoord);
		albedo.w = 1.0f;
	}

	// normal
	float3 normal = input.normal;
	if (HasNormalTexture())
	{
		// 일단 현재 sponza의 normal이 없다고 나온다.
		// 그리고 샘플링도 이렇게 하는 게 아닐 것이다.
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