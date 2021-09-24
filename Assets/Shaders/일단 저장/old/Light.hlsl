#include "common.hlsl"

Pixel_PosTex mainVS(Vertex_PosTex input)
{
	Pixel_PosTex output = (Pixel_PosTex)0;

	output.position.w = 0;
	output.position = mul(input.position, g_viewProjOrtho);

	output.uv = input.uv;

	return output;
}

Texture2D tex_albedo	: register(t0);
Texture2D tex_normal	: register(t1);
Texture2D tex_material	: register(t2);
Texture2D tex_depth		: register(t3);

SamplerState samplerPoint : register(s0);

cbuffer LightData : register(b1)
{
	float3 lightPosition;
	float lightRange;
	float3 lightDirection;
	float lightSpotConeOuter;
	float3 lightColor;
	float lightSpotConeinner;
}

float3 calcuDirectionalLight(float2 pos, float2 texCoord)
{
	float3 finalColor = (float3)0;

	int3 location3 = int3(pos.xy, 0);
	float3 normal = tex_normal.Load(location3).xyz;
	normal = normalize(normal *2.0f - 1.0f);	// 다시 -1.0 ~ 1.0으로 변경

	// depth값을 계산한 후 world position 획득
	float depth = tex_depth.Load(location3).x;
	float3 worldPos = GetWorldPositionFromDepth(depth, texCoord);
	
	// material
	float specExp = tex_material.Load(location3).x;
	float specIntensity = tex_material.Load(location3).y;

	// ambient
	float3 ambient = float3(0.0f, 0.0f, 0.0f);
	finalColor = ambient;

	// phong - diffuse
	float3 toLight = -normalize(lightDirection);
	float diffuse = saturate(dot(normal, toLight));
	finalColor += lightColor * diffuse;

	// blinn - specular
	// 아무래도 worldPos가 잘못된 것 같다.
	float3 toEye = normalize(g_cameraPos - worldPos);
	float3 halfWay = normalize(toEye + toLight);
	float NDotH = saturate(dot(halfWay, normal));
	//finalColor += lightColor * pow(NDotH, specExp) * specIntensity;

	if (worldPos.z >= 0.0f && worldPos.z <= 500.0f)
		finalColor = float3(1.0f, 0.0f, 0.0f);

	return finalColor;
}

float4 mainPS(Pixel_PosTex input) : SV_TARGET 
{
	//float4 albedo = tex_albedo.Sample(samplerPoint, input.uv);
	int3 location3 = int3(input.position.xy, 0);
	float4 albedo = tex_albedo.Load(location3);

	float3 lightDirectional = (float3)0;
	float3 lightPoint = (float3)0;
	float3 lightSpot = (float3)0;

#if DIRECTIONAL
	lightDirectional = calcuDirectionalLight(input.position.xy, input.uv);
#endif

#if POINT
#endif

#if SPOT
#endif

	float3 light = lightDirectional + lightPoint + lightSpot;

	return float4(albedo.xyz*light, albedo.w);
}