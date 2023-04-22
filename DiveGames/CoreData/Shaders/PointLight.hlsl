#include "ForwardLightCommon.hlsl"
#include "Samplers.hlsl"

float4 MainPS(PixelInput input) : SV_TARGET0
{
	float4 diffMap = DiffMapTex.Sample(DiffMapSampler, input.tex);

	float3 toLight = lightPos - input.worldPos;
	float3 toEye = cameraPos - input.worldPos;
	float distToLight = length(toLight);

	// Phong Diffuse
	toLight /= distToLight;
	float NDotL = saturate(dot(toLight, input.normal));
	float3 pointLightColor = lightColor * NDotL;

	// Blinn Specular
	toEye = normalize(toEye);
	float3 halfWay = normalize(toEye + toLight);
	float NDotH = saturate(dot(halfWay, input.normal));
	pointLightColor += lightColor * pow(NDotH, 250.0f) * 0.25f;

	// Attenution
	float distToLightNorm = 1.0f - saturate(distToLight * lightRange);
	float attn = distToLightNorm * distToLightNorm;
	pointLightColor *= materialDiffColor.xyz * attn;

	return diffMap * float4(pointLightColor, 1.0f);
}