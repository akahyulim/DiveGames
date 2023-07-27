#include "ForwardLightCommon.hlsl"

float4 MainPS(PixelInput input) : SV_TARGET0
{
	float4 diffuse = DiffuseMap.Sample(DiffuseMapSampler, input.tex);

	// Phong diffuse
	float NDotL = dot(-cbLightDirPS, input.normal);
	float3 dirLightColor = cbLightColorPS * saturate(NDotL);

	// Blinn specular
	float3 toEye = cbCameraPosPS - input.worldPos.xyz;
	toEye = normalize(toEye);
	float3 halfWay = normalize(toEye + -cbLightDirPS);
	float NDotH = saturate(dot(halfWay, input.normal));
	dirLightColor += cbLightColorPS * pow(NDotH, 250.0f) * 0.25f;

	return diffuse * float4(dirLightColor, 1.0f) * cbMaterialAlbedoColorPS;
}