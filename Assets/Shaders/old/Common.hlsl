/*
	����: ������( akahyulim@gmail.com )
*/

//= INCLUDES ==================
#include "Common_Buffer.hlsl"
#include "Common_Vertex.hlsl"
//=============================

// ���� ���
#define PI 3.1415926535897932384626433832795
#define INV_PI 1.0 / PI;
#define EPSILON 0.00000001


// ���� �ڷ���
float g_gamma = 2.2f;	// ��� ���ۿ� ���ԵǾ�� �Ѵ�.
float3 GammaToLinear(float3 color) { return pow(abs(color), g_gamma); }
float4 GammaToLinear(float4 color) { return pow(abs(color), g_gamma); }
float3 LinearToGamma(float3 color) { return pow(abs(color), 1.0f / g_gamma); }
float4 LinearToGamma(float4 color) { return pow(abs(color), 1.0f / g_gamma); }


float ConvertZToLinearDepth(float depth)
{
	return g_perspectiveValues.z / (depth + g_perspectiveValues.w);
}

float3 CalcuWorldPosition(float depth, float2 texCoord)
{
	// 0 ~ 1�� -1 ~ 1�� ��ȯ
	float x = texCoord.x * 2.0f - 1.0f;
	float y = (1.0f - texCoord.y) * 2.0f - 1.0f;
	float4 posClip = float4(x, y, depth, 1.0f);
	float4 posWorld = mul(posClip, g_viewProjInverse);
	return posWorld.xyz / posWorld.w;
}

float3 GetWorldPositionFromDepth(float depth, float2 texCoord)
{
	float z = ConvertZToLinearDepth(depth);
	return CalcuWorldPosition(z, texCoord);
}

// ��Ÿ
bool IsSaturated(float value) { return value == saturate(value); }
bool IsSaturated(float2 value) { return IsSaturated(value.x) && IsSaturated(value.y); }
bool IsSaturated(float3 value) { return IsSaturated(value.x) && IsSaturated(value.y) && IsSaturated(value.z); }