#include "Common.hlsl"

// 이건 클립 스페이스다.( -1.0 ~ 1.0 )
static const float2 arrBasePos[4] = {
	float2(-1.0, 1.0),		// 좌상
	float2(1.0, 1.0),		// 우상
	float2(-1.0, -1.0),		// 좌하
	float2(1.0, -1.0),		// 우하
};

/////////////////////////////////////////////////////////////////////////////
// Vertex shader
/////////////////////////////////////////////////////////////////////////////
struct VS_OUTPUT
{
	// 책에서는 각각 position, uv다.
	float4 Position : SV_Position; // vertex position 
	float2 cpPos	: TEXCOORD0;
};

VS_OUTPUT mainVS(uint VertexID : SV_VertexID)
{
	VS_OUTPUT Output;

	// 0 ~ 6. 즉, 삼각형 두 개?
	// x, y는 값이 같다. 
	Output.Position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);	// projection space
	Output.cpPos = Output.Position.xy;								// clip space

	// 복수의 문서에서 projection 결과가 clip space라 한다...
	// 즉, 위의 구분은 4차원과 2차원으로 이루어진듯 하다.

	// 여하튼 rastrize 후 값이 달라진다.

	return Output;
}

// diffuse texture and sampler
Texture2D DiffuseTexture	: register(t0);
SamplerState LinearSampler	: register(s0);

float4 mainPS(VS_OUTPUT In) : SV_TARGET
{
	// 좌표계가 다른가?
	// 뒤집어졌다.
	float3 diffuse = DiffuseTexture.Sample(LinearSampler, In.cpPos);

	return float4(diffuse, 1.0f);
}