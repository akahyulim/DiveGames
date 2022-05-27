#include "common_vertex.hlsl"
#include "common_buffer.hlsl"
#include "common_texture.hlsl"

// GBuffer
Texture2D<float> GBufferDepth		: register(t0);
Texture2D<float4> GBufferAlbedo		: register(t1);
Texture2D<float4> GBufferNormal		: register(t2);
Texture2D<float4> GBufferMaterial	: register(t3);

// Samplers
SamplerState PointSampler	: register(s0);
SamplerState LinearSampler	: register(s1);

struct Material
{
	float4 albedo;
	float3 normal;
};

struct SurfaceData
{
	float linearDepth;
	float3 albedo;
	float3 normal;
};

float ConvertZToLinearDepth(float depth)
{
	float linearDepth = g_PerspectiveValues.z / (depth + g_PerspectiveValues.w);
	return linearDepth;
}

float3 CalcWorldPos(float2 csPoint, float linearDepth)
{
	float4 pos;

	// View Space
	pos.xy	= csPoint.xy * g_PerspectiveValues.xy * linearDepth;
	pos.z	= linearDepth;
	pos.w	= 1.0f;

	// World Space
	return mul(pos, g_ViewInv).xyz;
}

SurfaceData UnpackGBuffer(int2 location)
{
	SurfaceData output;
	int3 location3 = int3(location, 0);

	float depth = GBufferDepth.Load(location3).x;
	output.linearDepth = ConvertZToLinearDepth(depth);

	float4 color = GBufferAlbedo.Load(location3);
	output.albedo = color.xyz;

	float3 normal = GBufferNormal.Load(location3).xyz;
	output.normal = normalize(normal.xyz * 2.0 - 1.0);

	return output;
}