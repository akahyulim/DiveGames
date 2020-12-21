#include "gbuffer.hlsl"

// textures & sampler
// 얘네도 이름이 같으면 안된다. include때문인듯
Texture2D tex_Albedo			: register(t0);
Texture2D tex_Normal			: register(t1);
Texture2D tex_Depth				: register(t2);

SamplerState sampler_Test		: register(s0);
//SamplerState sampler_Linear		: register(s1);

static const float2 arrBasePos[4] = {
	float2(1.0, 1.0),
	float2(1.0, -1.0),
	float2(-1.0, 1.0),
	float2(-1.0, -1.0),
};

static const float2 arrUV[4] = {
	float2(1.0, 0.0),
	float2(1.0, 1.0),
	float2(0.0, 0.0),
	float2(0.0, 1.0),
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float2 uv	: TEXCOORD0;
};

VS_OUTPUT mainVS(uint VertexID : SV_VertexID)
{
	VS_OUTPUT output;

	output.position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
	output.uv = arrUV[VertexID % 4].xy;

	return output;
}


float3 CalcuDirectionalLight(float2 texCoord)
{
	// light position, diffuse color 필요	=> 추후 component에서 constant buffer로 전달받아야 한다.
	float3 posLight = float3(-50.0f, 100.0f, -100.0f);
	float3 lightColor = float3(1.0f, 1.0f, 1.0f);
	
	float3 finalColor = (float3)0;

	float3 normal = tex_Normal.Sample(sampler_Test, texCoord).xyz;
	normal = normalize(normal);

	// depth값을 계산한 후 world position 획득
	float depth = tex_Depth.Sample(sampler_Test, texCoord).r;
	float3 worldPos = GetWorldPositionFromDepth(depth, texCoord);

	// phong - diffuse
	float3 toLight = normalize(posLight - worldPos);
	float diffuse = saturate(dot(normal, toLight));
	if (diffuse > 0.0f)
		finalColor = lightColor * diffuse;

	// blinn - specular
	float3 toEye = normalize(g_cameraPos - worldPos);
	float3 halfWay = normalize(toEye + toLight);
	finalColor += lightColor * pow(saturate(dot(halfWay, normal)), 20.0f);

	return saturate(finalColor);
}

float4 mainPS(VS_OUTPUT input) : SV_TARGET
{
	float4 albedo = tex_Albedo.Sample(sampler_Test, input.uv);
	float3 light = CalcuDirectionalLight(input.uv);

	return float4(light*albedo.xyz, albedo.w);
}