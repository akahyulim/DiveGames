#include "Common.hlsl"

// legacy light

struct VS_INPUT
{
    float4 position     : POSITION0;
    float2 tex          : TEXCOORD0;
    float3 normal       : NORMAL0;
    float3 tangent      : TANGENT0;
    float3 bitangent    : BINORMAL0;
};

struct VS_OUTPUT
{
    float4 position     : SV_POSITION;
    float2 tex          : TEXCOORD0;
    float3 worldPos     : TEXCOORD1;
    float3 normal       : NORMAL0;
    float3 tangent      : TANGENT0;
    float3 bitangent    : BINORMAL0;
};

VS_OUTPUT MainVS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    input.position.w = 1.0f;
    output.position = mul(input.position, cbFrame.world);
    output.worldPos = output.position.xyz;
    output.position = mul(output.position, mul(cbFrame.view, cbFrame.projection));
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3)cbFrame.world);
    output.normal = normalize(output.normal);
    output.tangent = mul(input.tangent, (float3x3) cbFrame.world);
    output.tangent = normalize(output.tangent);
    output.bitangent = mul(input.bitangent, (float3x3) cbFrame.world);
    output.bitangent = normalize(output.bitangent);
    
    return output;
}

float3 CalcuDirLight(float3 worldPos, float3 normal, float3 materialDiff)
{
    // Phong diffuse
    float NDotL = saturate(dot(-cbLight.direction, normal));
    float3 finalColor = cbLight.color * NDotL;

	// Blinn specular
    float3 toEye = cbCamera.position - worldPos;
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + -cbLight.direction);
    float NDotH = saturate(dot(halfWay, normal));
    
    finalColor += cbLight.color * pow(NDotH, 250.0f) * 0.25f;
    
    return finalColor * materialDiff;
}

float3 CalcuPointLight(float3 worldPos, float3 normal, float3 materialDiff)
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
    
    finalColor *= materialDiff * attn;
    
    return finalColor;
}

float3 CalcuSpotLight(float3 worldPos, float3 normal, float3 materialDiff)
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
    
    finalColor *= materialDiff * distAttn * coneAtt;
    
    return finalColor;
}

float4 MainPS(VS_OUTPUT input) : SV_TARGET
{
    float4 diff;
    if (!HasDiffuseTexture())
        diff = cbMaterial.color;
    else
        diff = DiffuseMap.Sample(DiffuseMapSampler, input.tex);
    
    float3 normal = input.normal;
    if (HasNormalTexture())
    {
        float4 bumpMap = NormalMap.Sample(NormalMapSampler, input.tex);
        bumpMap = (bumpMap * 2.0f) - 1.0f;

        normal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.bitangent) + (bumpMap.z * input.normal));
    }

    float3 ambient = float3(0.2f, 0.2f, 0.2f);
    
    float3 lightColor;
    if (IsDirectionalLight())
        lightColor = CalcuDirLight(input.worldPos, normal, diff.xyz) + (ambient * diff.xyz);
    else if (IsPointLight())
        lightColor = CalcuPointLight(input.worldPos, normal, diff.xyz);
    else if (IsSpotLight())
        lightColor = CalcuSpotLight(input.worldPos, normal, diff.xyz);
    
    return float4(lightColor, diff.a);
}