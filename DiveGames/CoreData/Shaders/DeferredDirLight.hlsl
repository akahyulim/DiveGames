#include "ConstantBuffers.hlsl"
#include "Samplers.hlsl"

// 이건 클립 스페이스다.( -1.0 ~ 1.0 )
static const float2 arrBasePos[4] = 
{
	float2(-1.0, 1.0),		// 좌상
	float2(1.0, 1.0),		// 우상
	float2(-1.0, -1.0),		// 좌하
	float2(1.0, -1.0),		// 우하
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 cpPos : TEXCOORD0;
};

// 책과 달리 0 ~ 3이 맞는 듯 보인다.
VS_OUTPUT MainVS(uint VertexID : SV_VERTEXID)
{
	VS_OUTPUT output;

	output.position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
	output.cpPos = output.position.xy;

	return output;
}

float4 frameRender : SV_TARGET0;

float4 MainPS(VS_OUTPUT input) : SV_TARGET0
{
	// Diff Color
	int3 location3 = int3(input.position.xy, 0);
	float4 diffMap = ColorSpecIntTex.Load(location3);

	// Linear Depth
	float depth = DepthTex.Load(location3).x;
	float linearDepth = cbPerspectiveValuePS.z / (depth + cbPerspectiveValuePS.w);

	// World Position
	float4 position;
	position.xy = input.cpPos.xy * cbPerspectiveValuePS.xy * linearDepth;
	position.z = linearDepth;
	position.w = 1.0f;
	position = mul(position, cbViewInvPS);

	// Normal
	float3 normal;
	normal = NormalTex.Load(location3);
	normal = normalize(normal * 2.0f - 1.0f);

	// Phong diffuse
	float NDotL = dot(-cbLightDirPS, normal);
	float3 dirLightColor = cbLightColorPS * saturate(NDotL);

	// Blinn specular
	float3 toEye = cbCameraPosPS - position.xyz;
	toEye = normalize(toEye);
	float3 halfWay = normalize(toEye + -cbLightDirPS);
	float NDotH = saturate(dot(halfWay, normal));
	dirLightColor += cbLightColorPS * pow(NDotH, 250.0f) * 0.25f;

	return diffMap;// *float4(dirLightColor, 1.0f)* cbMaterialAlbedoColorPS;
}