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

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 cpPos : TEXCOORD0;
};

[domain("quad")]
DS_OUTPUT MainDS(HS_CONSTANT_DATA_OUTPUT input, float2 uv : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 4> quad)
{
    // Transform the UV's into clip-space
	// 0 ~ 1 좌표계를 -1 ~ 1 좌표계로 변환
    float2 posClipSpace = uv.xy * 2.0 - 1.0;

	// Find the absulate maximum distance from the center
    float2 posClipSpaceAbs = abs(posClipSpace.xy);
    float maxLen = max(posClipSpaceAbs.x, posClipSpaceAbs.y);

	// Generate the final position in clip-space
	// 출력 제어점은 quad[0] 하나만 사용한다. 값은 1.0, 1.0, 1.0 이다. => 어차피 배열엔 전부 동일한 값이다.
	// 결국 normDir는 정규화된 반구 형태가 된다.
    float3 normDir = normalize(float3(posClipSpace.xy, (maxLen - 1.0)) * quad[0].HemiDir);
    float4 posLS = float4(normDir.xyz, 1.0);
	
	// Transform all the way to projected space
    // 사이즈, 위치, 뷰, 프로젝션 모두 적용
    DS_OUTPUT output;
    output.position = mul(posLS, cbLightDomain.lightProjection);

	// Store the clip space position
    output.cpPos = output.position.xy / output.position.w;

    return output;
}

float3 CalcuPointLight(float3 worldPos, float3 normal, float3 diffColor)
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
    
    // shadow
    float shadowAtt = 1.0f;
    //if (cbLightPixel.shadowEnabled)
    //   shadowAtt = PointShadowPCF(worldPos - cbLightPixel.position);
    
    finalColor *= diffColor * attn * shadowAtt;
    
    return finalColor;
}

float4 MainPS(DS_OUTPUT input) : SV_TARGET
{
    // Diff Color
    int3 location3 = int3(input.position.xy, 0);
    float4 diffColor = DiffuseTex.Load(location3);
    
	// Linear Depth
    float depth = DepthTex.Load(location3).x;
    float linearDepth = cbCameraPixel.perspectiveValue.z / (depth + cbCameraPixel.perspectiveValue.w);

	// World Position   
    float4 position;
    position.xy = input.cpPos.xy * cbCameraPixel.perspectiveValue.xy * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    position.xyz = mul(position, cbCameraPixel.viewInverse).xyz;
    
	// Normal
    float3 normal;
    normal = NormalTex.Load(location3);
    normal = normalize(normal * 2.0f - 1.0f);
    
     // 추후 상수버퍼를 통해 그림자 적용 여부를 전달받아야 한다.
    float3 lightColor = CalcuPointLight(position.xyz, normal, diffColor.rgb);
    
    return float4(lightColor.xyz, 1.0f);
}