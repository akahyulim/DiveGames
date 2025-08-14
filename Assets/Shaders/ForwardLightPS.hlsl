#include "Constants.hlsli"
#include "Resources.hlsli"

float3 CalcuDirLight(float3 worldPos, float3 normal, float3 diffColor)
{
    float3 finalColor;
    
    // Ambient
    float3 ambientColor = float3(0.05f, 0.05f, 0.05f);
    finalColor = ambientColor;
    
    // Phong diffuse
    float NDotL = saturate(dot(-cbLight.direction, normal));
    if (NDotL > 0.0f)
    {
        finalColor += cbLight.color * NDotL;
    }
    
	// Blinn specular
    float3 toEye = cbCamera.position - worldPos;
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + -cbLight.direction);
    float NDotH = saturate(dot(halfWay, normal));
    
    finalColor += cbLight.color * pow(NDotH, 250.0f) * 0.25f;
    
    return diffColor * finalColor;
}

float3 CalcuPointLight(float3 worldPos, float3 normal, float3 diffColor)
{
    float3 toLight = cbLight.position - worldPos;
    float3 toEye = cbCamera.position - worldPos;
    float distance = length(toLight);
    
    // phong diffuse
    toLight /= distance;
    float NDotL = saturate(dot(toLight, normal));
    float3 finalColor = cbLight.color * NDotL;
    
    // blinn specular
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + toLight);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += cbLight.color * pow(NDotH, 250.0f) * 0.25f;
    
    // attenuation
    float distToLightNorm = 1.0f - saturate(distance * cbLight.rangeRcp);
    float attn = distToLightNorm * distToLightNorm;
    
    finalColor *= diffColor * attn;
    
    return finalColor;
}

float3 CalcuSpotLight(float3 worldPos, float3 normal, float3 diffColor)
{
    float3 toLight = cbLight.position - worldPos;
    float3 toEye = cbCamera.position - worldPos;
    float distance = length(toLight);
    
    // phong diffuse
    toLight /= distance;
    float NDotL = saturate(dot(toLight, normal));
    float3 finalColor = cbLight.color * NDotL;
    
    // blinn specular
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + toLight);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += cbLight.color * pow(NDotH, 250.0f) * 0.25f;
    
    // attenuation
    float distToLightNorm = 1.0f - saturate(distance * cbLight.rangeRcp);
    float distAttn = distToLightNorm * distToLightNorm;
    
    // cone attenuation
    float cosAng = acos(dot(-cbLight.direction, toLight));
    float coneAtt = 1.0f * smoothstep(cbLight.outerConeAngle, cbLight.innerConeAngle, cosAng);
    
    float shadowAtt = 1.0f;
    //if(cbLight.shadowEnabled)
    //    shadowAtt = SpotShadowPCF(worldPos);
    
    finalColor *= diffColor * distAttn * coneAtt * shadowAtt;
    
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
    float4 diffColor;
 
    if (!HasDiffuseMap())
        diffColor = cbMaterial.diffuseColor;
    else
        diffColor = DiffuseMap.Sample(WrapLinearSampler, input.UV);

    float3 normal = input.Normal;

    float3 lightColor;
    if (IsDirectionalLight())
        lightColor = CalcuDirLight(input.WorldPos, normal, diffColor.rgb);
    else if (IsPointLight())
        lightColor = CalcuPointLight(input.WorldPos, normal, diffColor.rgb);
    else if (IsSpotLight())
        lightColor = CalcuSpotLight(input.WorldPos, normal, diffColor.rgb);
    
    //return float4(sqrt(lightColor.rgb), diffColor.a);
    return float4(lightColor.rgb, diffColor.a);
    //return diffColor;
    //return float4(normal * 0.5 + 0.5, 1.0);
}