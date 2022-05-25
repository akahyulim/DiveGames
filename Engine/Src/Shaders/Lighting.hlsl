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

Texture2D<float> DepthTexture		: register(t0);
Texture2D<float4> AlbedoTexture		: register(t1);
Texture2D<float4> NormalTexture		: register(t2);
Texture2D<float4> MaterialTexture	: register(t3);

struct Pixel_Output
{
	float4 FrameOutput : SV_TARGET0;
};

// pixel shade
// 일단 depth를 선형으로 출력한다.
Pixel_Output mainPS(Pixel_Input input)
{
	Pixel_Output output;

	int3 location3 = int3(input.Position.xy, 0);

	float depth = DepthTexture.Load(location3).x;
	depth = ConvertZToLinearDepth(depth);
	
	// 책에선 출력위해 각 요소의 값을 다시 계산했다.
	output.FrameOutput = float4(
		1.0 - saturate(depth / 75.0),
		1.0 - saturate(depth / 125.0),
		1.0 - saturate(depth / 200.0),
		1.0);

	// 결국
	// albedo는 화면의 uv에 맞게 색상을 저장하고
	// normal은 법선 벡터를 저장하고
	// depth는 깊이 값을 저장한 것이다.
	// 즉, 모든 요소를 개별 텍스쳐에 기록한다고 보면 된다.
	// 그리고 이를 이용해 light를 처리하는 것이다.

	return output;
}