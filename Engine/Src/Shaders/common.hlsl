#include "common_vertex.hlsl"
#include "common_buffer.hlsl"
#include "common_texture.hlsl"

SamplerState PointSampler	: register(s0);
SamplerState LinearSampler	: register(s1);

// 이하 공통 수학 함수
float ConvertZToLinearDepth(float depth)
{
	float linearDepth = g_PerspectiveValues.z / (depth + g_PerspectiveValues.w);
	return linearDepth;
}