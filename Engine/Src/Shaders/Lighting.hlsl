#include "common.hlsl"

// 이게 Draw에 전달된 4개의 vertex라 생각하면 된다.
static const float2 arrBasePos[4] =
{
	float2(-1.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(-1.0f, -1.0f),
	float2(1.0f, -1.0f),
};

struct Pixel_Input
{
	float4 Position : SV_POSITION;
	float2 cpPos	: TEXCOORD0;
};

Pixel_Input mainVS(uint VertexID : SV_VERTEXID)
{
	Pixel_Input output;

	output.Position = float4(arrBasePos[VertexID].xy, 0.0f, 1.0f);
	output.cpPos = output.Position.xy;

	return output;
}

struct Pixel_Output
{
	float4 FrameOutput : SV_TARGET0;
};

float3 CalcDirectionalLight(float3 pos, float depth, SurfaceData data)
{
	float3 dirLight = 0;

	float3 lightDir = -g_LightDir;

	// ambient
	float ambient = 0.1f;
	dirLight = g_LightColor * ambient;

	// phong diffuse
	float diffuse = saturate(dot(data.normal, lightDir));
	dirLight += g_LightColor * diffuse;

	// blinn specular
	float3 toEye = normalize(g_EyePos - pos);
	float3 halfWay = normalize(toEye + lightDir);
	float specular = pow(saturate(dot(data.normal, halfWay)), 250.f) * 0.25f;
	dirLight += g_LightColor * specular;

	return data.albedo * dirLight;
}

Pixel_Output mainPS(Pixel_Input input)
{
	Pixel_Output output;

	SurfaceData data = UnpackGBuffer(input.Position.xy);
	
	float depth = data.linearDepth;
	float3 pos = CalcWorldPos(input.cpPos, depth);

	float3 directionalLight = CalcDirectionalLight(pos, depth, data);

	output.FrameOutput = float4(directionalLight.xyz, 1.0f);

	return output;
}