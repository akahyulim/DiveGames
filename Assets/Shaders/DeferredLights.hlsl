#include "Common.hlsl"

// 이건 클립 스페이스다.( -1.0 ~ 1.0 )
static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0), // 좌상
	float2(1.0, 1.0), // 우상
	float2(-1.0, -1.0), // 좌하
	float2(1.0, -1.0), // 우하
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 cpPos : TEXCOORD0;
};

// 책과 달리 0 ~ 3이 맞는 듯 보인다.
VS_OUTPUT MainVS(uint VertexID : SV_VERTEXID)
{
    VS_OUTPUT output;

    output.position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
    output.cpPos = output.position.xy;

    return output;
}

float3 CalcuDirLight(float3 worldPos, float3 normal, float3 diff)
{
    float3 finalColor;
    
    // Ambient
    float3 ambientColor = float3(0.05f, 0.05f, 0.05f);
    finalColor = ambientColor;
    
    // Phong diffuse
    float NDotL = saturate(dot(-cbLightPixel.direction, normal));
    if (NDotL > 0.0f)
    {
        finalColor += cbLightPixel.color * NDotL;
    }
    
	// Blinn specular
    float3 toEye = cbCameraPixel.position - worldPos;
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + -cbLightPixel.direction);
    float NDotH = saturate(dot(halfWay, normal));
    
    finalColor += cbLightPixel.color *
    pow(NDotH, 250.0f) * 0.25f;
    
    return finalColor * diff;
}
float3 CalcuPointLight(float3 worldPos, float3 normal, float3 diff)
{
    float3 toLight = cbLightPixel.position - worldPos;
    float3 toEye = cbCameraPixel.position - worldPos;
    float distance = length(toLight);
    
    // phong diffuse
    toLight /= distance;
    float NDotL = saturate(dot(toLight, normal));
    float3 finalColor = cbLightPixel.color * NDotL;
    
    // blinn specular
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + toLight);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += cbLightPixel.color * pow(NDotH, 250.0f) * 0.25f;
    
    // attenuation
    float distToLightNorm = 1.0f - saturate(distance * cbLightPixel.rangeRcp);
    float attn = distToLightNorm * distToLightNorm;
    
    finalColor *= diff * attn;
    
    return finalColor;
}

float3 CalcuSpotLight(float3 worldPos, float3 normal, float3 diff)
{
    float3 toLight = cbLightPixel.position - worldPos;
    float3 toEye = cbCameraPixel.position - worldPos;
    float distance = length(toLight);
    
    // phong diffuse
    toLight /= distance;
    float NDotL = saturate(dot(toLight, normal));
    float3 finalColor = cbLightPixel.color * NDotL;
    
    // blinn specular
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + toLight);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += cbLightPixel.color * pow(NDotH, 250.0f) * 0.25f;
    
    // attenuation
    float distToLightNorm = 1.0f - saturate(distance * cbLightPixel.rangeRcp);
    float distAttn = distToLightNorm * distToLightNorm;
    
    // cone attenuation
    float cosAng = acos(dot(-cbLightPixel.direction, toLight));
    float coneAtt = 1.0f * smoothstep(cbLightPixel.outerConeAngle, cbLightPixel.innerConeAngle, cosAng);
    
    finalColor *= diff * distAttn * coneAtt;
    
    return finalColor;
}

float4 MainPS(VS_OUTPUT input) : SV_TARGET
{
	// Diff Color
    int3 location3 = int3(input.position.xy, 0);
    float4 diffMap = ColorSpecIntTex.Load(location3);
    
	// Linear Depth
    float depth = DepthTex.Load(location3).x;
    float linearDepth = cbCameraPixel.perspectiveValue.z / (depth + cbCameraPixel.perspectiveValue.w);

	// World Position
    float4 position;
    position.xy = input.cpPos.xy * cbCameraPixel.perspectiveValue.xy * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    position = mul(position, cbCameraPixel.viewInverse);

	// Normal
    float3 normal;
    normal = NormalTex.Load(location3);
    normal = normalize(normal * 2.0f - 1.0f);
    
    float3 lightColor;
    if (IsDirectionalLight())
        lightColor = CalcuDirLight(position.xyz, normal, diffMap.xyz);
    else if (IsPointLight())
        lightColor = CalcuPointLight(position.xyz, normal, diffMap.xyz);
    else if (IsSpotLight())
        lightColor = CalcuSpotLight(position.xyz, normal, diffMap.xyz);
    
    return float4(lightColor.xyz, 1.0f);
}