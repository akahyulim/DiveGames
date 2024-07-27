#include "Common.hlsl"

float4 MainVS() : SV_Position
{
    return float4(0.0, 0.0, 0.0, 1.0);
}

struct HS_CONSTANT_DATA_OUTPUT
{
    float edges[4]      : SV_TessFactor;
    float insides[2]    : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT SpotLightConstantHS()
{
    HS_CONSTANT_DATA_OUTPUT output;
    
    float tessFactor = 18.0;
    output.edges[0] = output.edges[1] = output.edges[2] = output.edges[3] = tessFactor;
    output.insides[0] = output.insides[1] = tessFactor;
    
    return output;
}

struct HS_OUTPUT
{
    float3 position : POSITION;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("SpotLightConstantHS")]
HS_OUTPUT MainHS()
{
    HS_OUTPUT output;
    output.position = float3(0.0, 0.0, 0.0);
    
    return output;
}

cbuffer cbDomain : register(b1)
{
    matrix World            : packoffset(c0);
    float SinOutterAngle    : packoffset(c4);
    float CosOutterAngle    : packoffset(c4.y);
};

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 cpPos    : TEXCOORD0;
};

#define CylinderPortion 0.2
#define ExpendAmount    (1.0 + CylinderPortion)

[domain("quad")]
DS_OUTPUT MainDS(HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
    // Transform the UV's into clip-space
    float2 posClipSpace = UV.xy * float2(2.0, -2.0) + float2(-1.0, 1.0);

	// Find the vertex offsets based on the UV
    float2 posClipSpaceAbs = abs(posClipSpace.xy);
    float maxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);

	// Force the cone vertices to the mesh edge
    float2 posClipSpaceNoCylAbs = saturate(posClipSpaceAbs * ExpendAmount);
    float maxLenNoCapsule = max(posClipSpaceNoCylAbs.x, posClipSpaceNoCylAbs.y);
    float2 posClipSpaceNoCyl = sign(posClipSpace.xy) * posClipSpaceNoCylAbs;

	// Convert the positions to half sphere with the cone vertices on the edge
    float3 halfSpherePos = normalize(float3(posClipSpaceNoCyl.xy, 1.0 - maxLenNoCapsule));

	// Scale the sphere to the size of the cones rounded base
    halfSpherePos = normalize(float3(halfSpherePos.xy * SinOutterAngle, CosOutterAngle));

	// Find the offsets for the cone vertices (0 for cone base)
    float cylinderOffsetZ = saturate((maxLen * ExpendAmount - 1.0) / CylinderPortion);

	// Offset the cone vertices to thier final position
    float4 posLS = float4(halfSpherePos.xy * (1.0 - cylinderOffsetZ), halfSpherePos.z - cylinderOffsetZ * CosOutterAngle, 1.0);

	// Transform all the way to projected space and generate the UV coordinates
    DS_OUTPUT Output;
    matrix lightProjection = mul(World, CameraViewProj);
    Output.position = mul(posLS, lightProjection);
    Output.cpPos = Output.position.xy / Output.position.w;

    return Output;
};

cbuffer cbPixel : register(b2)
{
    float3 LightColor       : packoffset(c0);
    float RangeRcp          : packoffset(c0.w);
    float3 LightPos         : packoffset(c1);
    float CosOutterCone     : packoffset(c1.w);
    float3 DirToLight       : packoffset(c2);
    float CosConeAttRange   : packoffset(c2.w);
    uint ShadowEnabled      : packoffset(c3);
};

float SpotShadowPCF(float3 position)
{
    float4 shadowMapPos = mul(float4(position, 1.0f), cbLightPixel.shadow);
    float3 uvd = shadowMapPos.xyz / shadowMapPos.w;
    uvd.xy = 0.5f * uvd.xy + 0.5f;
    uvd.y = 1.0f - uvd.y;
    
    return SpotShadowMap.SampleCmpLevelZero(PcfSampler, uvd.xy, uvd.z);
}

float3 CalcuSpotLight(float3 worldPos, float3 normal, float3 diffColor)
{
    float3 toLight = LightPos - worldPos;
    float3 toEye = CameraPos - worldPos;
    float distToLight = length(toLight);
    
    // phong diffuse
    toLight /= distToLight;
    float NDotL = saturate(dot(toLight, normal));
    float3 finalColor = LightColor * NDotL;
    
    // blinn specular
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + toLight);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += LightColor * pow(NDotH, 250.0f) * 0.25f;
    
    // cone attenuation
    //float cosAng = acos(dot(-DirToLight, toLight));
    //float coneAtt = 1.0f * smoothstep(OutterConeCos, InnerConeAttCos, cosAng);
    float cosAng = dot(-DirToLight, toLight);
    float coneAtt = saturate((cosAng - CosOutterCone) / CosConeAttRange);
    coneAtt *= coneAtt;
    
    // attenuation
    float distToLightNorm = 1.0f - saturate(distToLight * RangeRcp);
    float distAttn = distToLightNorm * distToLightNorm;
    
    // shadow
    float shadowAtt = 1.0f;
    //if (ShadowEnabled)
    //    shadowAtt = SpotShadowPCF(worldPos);
    
    finalColor *= diffColor * distAttn * coneAtt; // * shadowAtt;
    
    return finalColor;
}

float4 MainPS(DS_OUTPUT input) : SV_TARGET
{
    // Diff LightColor
    int3 location3 = int3(input.position.xy, 0);
    float4 diffColor = DiffuseTex.Load(location3);
    
	// Linear Depth
    float depth = DepthTex.Load(location3).x;
    float linearDepth = CameraPerspectiveValue.z / (depth + CameraPerspectiveValue.w);
    
    // World LightPos
    float4 position;
    position.xy = input.cpPos.xy * CameraPerspectiveValue.xy * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    position.xyz = mul(position, CameraViewInverse).xyz;

	// Normal
    float3 normal;
    normal = NormalTex.Load(location3);
    normal = normalize(normal * 2.0 - 1.0);
    
    float3 lightColor = CalcuSpotLight(position.xyz, normal, diffColor.rgb);
    
    return float4(lightColor.xyz, 1.0);
}