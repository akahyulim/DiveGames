#include "Constants.hlsli"
#include "Resources.hlsli"

float3 CalcuDirLight(float3 worldPos, float3 normal, LightData data)
{
    float3 finalColor = {0.0, 0.0, 0.0};
    
    // Phong diffuse
    float NDotL = saturate(dot(-data.direction, normal));
    if (NDotL > 0.0f)
    {
        finalColor += data.color * NDotL;
    }

	// Blinn specular
    float3 toEye = cbCamera.position - worldPos;
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + -data.direction);
    float NDotH = saturate(dot(halfWay, normal));
    
    finalColor += data.color * pow(NDotH, 250.0f) * 0.25f;
    
    return finalColor;
}

float3 CalcuPointLight(float3 worldPos, float3 normal, LightData data)
{
    float3 toLight = data.position - worldPos;
    float3 toEye = cbCamera.position - worldPos;
    float distance = length(toLight);
    
    // phong diffuse
    toLight /= distance;
    float NDotL = saturate(dot(toLight, normal));
    float3 finalColor = data.color * NDotL;
    
    // blinn specular
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + toLight);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += data.color * pow(NDotH, 250.0f) * 0.25f;
    
    // attenuation
    float distToLightNorm = 1.0f - saturate(distance * data.rangeRcp);
    float attn = distToLightNorm * distToLightNorm;
    
    finalColor *= attn;
    return finalColor;
}

float3 CalcuSpotLight(float3 worldPos, float3 normal, LightData data)
{
    float3 toLight = data.position - worldPos;
    float distance = length(toLight);
    float3 lightDir = normalize(toLight);

    // Spot factor: 픽셀 방향과 빛 방향 사이의 각도
    float spotFactor = dot(lightDir, -data.direction);

    // Inner/Outer cutoff
    float innerCutoff = cos(data.innerAngle);
    float outerCutoff = cos(data.outerAngle);

    // Spot attenuation 계산
    float spotAttn = saturate((spotFactor - outerCutoff) / (innerCutoff - outerCutoff));
    spotAttn *= spotAttn; // falloff 부드럽게

    // 조명 영향 없음
    if (spotAttn <= 0.0f)
        return float3(0.0, 0.0, 0.0);

    // Phong diffuse
    float NDotL = saturate(dot(lightDir, normal));
    float3 finalColor = data.color * NDotL;

    // Blinn specular
    float3 toEye = normalize(cbCamera.position - worldPos);
    float3 halfWay = normalize(toEye + lightDir);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += data.color * pow(NDotH, 250.0f) * 0.25f;

    // 거리 기반 감쇠
    float distNorm = 1.0f - saturate(distance * data.rangeRcp);
    float attn = distNorm * distNorm;

    // 최종 색상
    finalColor *= attn * spotAttn;
    return finalColor;
}

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

float4 MainPS(PSInput input) : SV_TARGET
{
    float3 normal = input.Normal;
    float4 diffColor = HasDiffuseMap() ?
        DiffuseMap.Sample(WrapLinearSampler, input.UV) : cbMaterial.diffuseColor;
    float3 lightColor = cbForwardLight.ambientColor;

    for (int i = 0; i < cbForwardLight.lightCount; ++i)
    {
        switch (cbForwardLight.lights[i].type)
        {
            case 0: lightColor += CalcuDirLight(input.WorldPos, normal, cbForwardLight.lights[i]); break;
            case 1: lightColor += CalcuPointLight(input.WorldPos, normal, cbForwardLight.lights[i]); break;
            case 2: lightColor += CalcuSpotLight(input.WorldPos, normal, cbForwardLight.lights[i]); break;
        }
    }

    return float4(diffColor.xyz * lightColor, diffColor.a);
}
