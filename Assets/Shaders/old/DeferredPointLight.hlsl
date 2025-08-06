#include "Common.hlsl"


float4 MainVS() : SV_Position
{
    return float4(0.0, 0.0, 0.0, 1.0);
}

struct HS_CONSTANT_DATA_OUTPUT
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT ConstantHS()
{
    HS_CONSTANT_DATA_OUTPUT output;
    
    float tessFactor = 18.0;
    output.edges[0] = output.edges[1] = output.edges[2] = output.edges[3] = tessFactor;
    output.inside[0] = output.inside[1] = tessFactor;
    
    return output;
}

struct HS_OUTPUT
{
    float3 HemiDir : POSITION;
};

static const float3 HemilDir[2] =
{
    float3(1.0, 1.0, 1.0),
	float3(-1.0, 1.0, -1.0)
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
HS_OUTPUT MainHS(uint patchID : SV_PrimitiveID)
{
    HS_OUTPUT output;
    output.HemiDir = HemilDir[patchID]; 
    
    return output;
}

cbuffer cbDomain : register(b1)
{
    matrix World : packoffset(c0);
};

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 cpPos : TEXCOORD0;
};

[domain("quad")]
DS_OUTPUT MainDS(HS_CONSTANT_DATA_OUTPUT input, float2 uv : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
    // Transform the UV's into clip-space
	// 0 ~ 1 ��ǥ�踦 -1 ~ 1 ��ǥ��� ��ȯ
    float2 posClipSpace = uv.xy * 2.0 - 1.0;

	// Find the absulate maximum distToLight from the center
    float2 posClipSpaceAbs = abs(posClipSpace.xy);
    float maxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);

	// Generate the final position in clip-space
	// ��� �������� quad[0] �ϳ��� ����Ѵ�. ���� 1.0, 1.0, 1.0 �̴�. => ������ �迭�� ���� ������ ���̴�.
	// �ᱹ normDir�� ����ȭ�� �ݱ� ���°� �ȴ�.
    float3 normDir = normalize(float3(posClipSpace.xy, (maxLen - 1.0)) * quad[0].HemiDir);
    float4 posLS = float4(normDir.xyz, 1.0);
	
	// Transform all the way to projected space
    // ������, ��ġ, ��, �������� ��� ����
    DS_OUTPUT output;
    matrix lightProjection = mul(World, CameraViewProj);
    output.position = mul(posLS, lightProjection);

	// Store the clip space position
    output.cpPos = output.position.xy / output.position.w;

    return output;
}

cbuffer cbPixel : register(b2)
{
    float3 LightColor               : packoffset(c0);
    float LightRangeRcp             : packoffset(c0.w);
    float3 LightPos                 : packoffset(c1);
    uint ShadowEnabled              : packoffset(c1.w);
    float2 LightPerspectiveValues   : packoffset(c2);
};

float PointShadowPCF(float3 toPixel)
{
    float3 toPixelAbs = abs(toPixel);
    float z = max(toPixelAbs.x, max(toPixelAbs.y, toPixelAbs.z));
    float depth = (LightPerspectiveValues.x * z + LightPerspectiveValues.y) / z;
    
    return PointShadowMap.SampleCmpLevelZero(PcfSampler, toPixel, depth);
}

float3 CalcuPointLight(float3 worldPos, float3 normal, float3 diffColor)
{
    float3 toLight = LightPos - worldPos;
    float3 toCamera = CameraPos - worldPos;
    float distToLight = length(toLight);
    
    // phong diffuse
    toLight /= distToLight;
    float NDotL = saturate(dot(toLight, normal));
    float3 finalColor = diffColor * NDotL;
    
    // blinn specular
    toCamera = normalize(toCamera);
    float3 halfWay = normalize(toCamera + toLight);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += pow(NDotH, 250.0) * 0.25;
    
    // attenuation
    float distToLightNorm = 1.0 - saturate(distToLight * LightRangeRcp);
    float distAtt = distToLightNorm * distToLightNorm;
    
    // shadow
    float shadowAtt = 1.0;
    if (ShadowEnabled == 1)   
        shadowAtt = PointShadowPCF(worldPos - LightPos);
    
    finalColor *= LightColor * distAtt * shadowAtt;
    
    return finalColor;
}

float4 MainPS(DS_OUTPUT input) : SV_TARGET0
{
    // Diff Color
    int3 location3 = int3(input.position.xy, 0);    // 0�� mipLevel. ��, �������� ���� ����
    float4 diffColor = DiffuseTex.Load(location3);
    
	// Linear Depth
    float depth = DepthTex.Load(location3).x;
    float linearDepth = CameraPerspectiveValue.z / (depth + CameraPerspectiveValue.w);
    
	// World LightPos
    float4 position;
    position.xy = input.cpPos.xy * CameraPerspectiveValue.xy * linearDepth;
    position.z = linearDepth;
    position.w = 1.0;
    position.xyz = mul(position, CameraViewInverse).xyz;
    
	// Normal
    float3 normal;
    normal = NormalTex.Load(location3);
    normal = normalize(normal * 2.0 - 1.0);
    
     // ���� ������۸� ���� �׸��� ���� ���θ� ���޹޾ƾ� �Ѵ�.
    float3 lightColor = CalcuPointLight(position.xyz, normal, diffColor.rgb);
    
    return float4(lightColor.xyz, 1.0);
}