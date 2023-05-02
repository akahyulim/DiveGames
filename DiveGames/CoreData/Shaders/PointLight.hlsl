#include "ForwardLightCommon.hlsl"

float4 MainPS(PixelInput input) : SV_TARGET0
{
	float4 diffMap = DiffMapTex.Sample(DiffMapSampler, input.tex);

	float3 toLight = cbLightPosPS - input.worldPos;
	float3 toEye = cbCameraPosPS - input.worldPos;
	float distToLight = length(toLight);

	// Phong Diffuse
	toLight /= distToLight;
	float NDotL = saturate(dot(toLight, input.normal));
	float3 pointLightColor = cbLightColorPS * NDotL;

	// Blinn Specular
	toEye = normalize(toEye);
	float3 halfWay = normalize(toEye + toLight);
	float NDotH = saturate(dot(halfWay, input.normal));
	pointLightColor += cbLightColorPS * pow(NDotH, 250.0f) * 0.25f;

	// Attenution
	float distToLightNorm = 1.0f - saturate(distToLight * cbLightRangePS);
	float attn = distToLightNorm * distToLightNorm;
	pointLightColor *= cbMaterialDiffColorPS.xyz * attn;

	return diffMap * float4(pointLightColor, 1.0f);
}