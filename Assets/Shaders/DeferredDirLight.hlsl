#include "Common.hlsl"

// �̰� Ŭ�� �����̽���.( -1.0 ~ 1.0 )
static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0),  // �»�
	float2(1.0, 1.0),   // ���
	float2(-1.0, -1.0), // ����
	float2(1.0, -1.0),  // ����
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 cpPos : TEXCOORD0;
};

// å�� �޸� 0 ~ 3�� �´� �� ���δ�.
VS_OUTPUT MainVS(uint VertexID : SV_VERTEXID)
{
    VS_OUTPUT output;

    output.position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
    output.cpPos = output.position.xy;

    return output;
}

// �������� �׸��� ���� ���θ� ���޹޵��� ����.
float3 CalcuDirLight(float3 worldPos, float3 normal, float3 diffColor, bool shadow)
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
    
    finalColor += cbLightPixel.color * pow(NDotH, 250.0f) * 0.25f;
    
    return diffColor * finalColor;
}

float4 MainPS(VS_OUTPUT input) : SV_TARGET
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
    
    // ���� ������۸� ���� �׸��� ���� ���θ� ���޹޾ƾ� �Ѵ�.
    float3 lightColor = CalcuDirLight(position.xyz, normal, diffColor.rgb, false);
    
    return float4(lightColor.xyz, 1.0f);
}