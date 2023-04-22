#include "ForwardLightCommon.hlsl"
#include "Samplers.hlsl"

float4 MainPS(PixelInput input) : SV_TARGET0
{
	float4 diffMap = DiffMapTex.Sample(DiffMapSampler, input.tex);

	// Phong diffuse
	float NDotL = dot(-lightDir, input.normal);
	float3 dirLightColor = lightColor * saturate(NDotL);

	// Blinn specular
	float3 toEye = cameraPos - input.worldPos.xyz;
	toEye = normalize(toEye);
	float3 halfWay = normalize(toEye + -lightDir);
	float NDotH = saturate(dot(halfWay, input.normal));
	dirLightColor += lightColor * pow(NDotH, 250.0f) * 0.25f;

	return diffMap * float4(dirLightColor, 1.0f) * materialDiffColor;
}