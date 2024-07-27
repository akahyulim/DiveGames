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
    float3 Color        : packoffset(c0);
    float3 ToLight      : packoffset(c1);
    uint ShadowEnabled  : packoffset(c1.w);
};

// �������� �׸��� ���� ���θ� ���޹޵��� ����.
float3 CalcuDirLight(float3 worldPos, float3 normal, float3 diffColor, bool shadow)
{
    float3 finalColor;
    
    // Ambient
    float3 ambientColor = float3(0.05f, 0.05f, 0.05f);
    finalColor = ambientColor;
    
    // Phong diffuse
    float NDotL = saturate(dot(-ToLight, normal)); //-cbLightPixel.ToLight, normal));
    if (NDotL > 0.0f)
    {
        finalColor += Color * NDotL; //cbLightPixel.Color * NDotL;
    }
    
	// Blinn specular
    float3 toEye = CameraPos - worldPos; //cbCameraPixel.position - worldPos;
    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + -ToLight); //-cbLightPixel.direction);
    float NDotH = saturate(dot(halfWay, normal));
    
    finalColor += Color * pow(NDotH, 250.0) * 0.25; //cbLightPixel.Color * pow(NDotH, 250.0f) * 0.25f;
    
    return diffColor * finalColor;
}

float4 MainPS(VS_OUTPUT input) : SV_TARGET
{
	// Diff Color
    int3 location3 = int3(input.position.xy, 0);
    float4 diffColor = DiffuseTex.Load(location3);
    
	// Linear Depth
    float depth = DepthTex.Load(location3).x;
    float linearDepth = CameraPerspectiveValue.z / (depth + CameraPerspectiveValue.w); //cbCameraPixel.perspectiveValue.z / (depth + cbCameraPixel.perspectiveValue.w);

	// World Position
    float4 position;
    position.xy = input.cpPos.xy * CameraPerspectiveValue.xy * linearDepth; //cbCameraPixel.perspectiveValue.xy * linearDepth;
    position.z = linearDepth;
    position.w = 1.0f;
    position.xyz = mul(position, CameraViewInverse).xyz; //cbCameraPixel.viewInverse).xyz;

	// Normal
    float3 normal;
    normal = NormalTex.Load(location3);
    normal = normalize(normal * 2.0f - 1.0f);
    
    // ���� ������۸� ���� �׸��� ���� ���θ� ���޹޾ƾ� �Ѵ�.
    float3 lightColor = CalcuDirLight(position.xyz, normal, diffColor.rgb, false);
    
    return float4(lightColor.xyz, 1.0f);
}