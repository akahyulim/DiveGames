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

cbuffer cbPixel : register(b2)
{
    float3 LightColor   : packoffset(c0);
    float3 ToLight      : packoffset(c1);
    uint ShadowEnabled  : packoffset(c1.w);
};

// �������� �׸��� ���� ���θ� ���޹޵��� ����.
float3 CalcuDirLight(float3 worldPos, float3 normal, float3 diffColor, bool shadow)
{
    float3 finalColor;
    
    // Ambient
    float3 ambientColor = float3(0.05f, 0.05f, 0.05f);
    
    // Phong diffuse
    float NDotL = saturate(dot(ToLight, normal));
    finalColor = diffColor * NDotL;
    
	// Blinn specular
    float3 toCamera = CameraPos - worldPos;
    toCamera = normalize(toCamera);
    float3 halfWay = normalize(toCamera + ToLight);
    float NDotH = saturate(dot(halfWay, normal));
    
    finalColor += pow(NDotH, 250.0) * 0.25;
    
    return LightColor * finalColor;
}

float4 MainPS(VS_OUTPUT input) : SV_TARGET0
{
	// Diff LightColor
    int3 location3 = int3(input.position.xy, 0);
    float4 diffColor = DiffuseTex.Load(location3);
    
	// Linear Depth
    float depth = DepthTex.Load(location3).x;
    float linearDepth = CameraPerspectiveValue.z / (depth + CameraPerspectiveValue.w);
    
	// World Position
    float4 position;
    position.xy = input.cpPos.xy * CameraPerspectiveValue.xy * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    position.xyz = mul(position, CameraViewInverse).xyz;

	// Normal
    float3 normal;
    normal = NormalTex.Load(location3);
    normal = normalize(normal * 2.0f - 1.0f);
    
    // ���� ������۸� ���� �׸��� ���� ���θ� ���޹޾ƾ� �Ѵ�.
    float3 lightColor = CalcuDirLight(position.xyz, normal, diffColor.rgb, false);
    
    return float4(lightColor.xyz, 1.0f);
}