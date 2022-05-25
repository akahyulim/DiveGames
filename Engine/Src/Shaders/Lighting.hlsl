#include "common.hlsl"

// �̰� Draw�� ���޵� 4���� vertex�� �����ϸ� �ȴ�.
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
// �ϴ� depth�� �������� ����Ѵ�.
Pixel_Output mainPS(Pixel_Input input)
{
	Pixel_Output output;

	int3 location3 = int3(input.Position.xy, 0);

	float depth = DepthTexture.Load(location3).x;
	depth = ConvertZToLinearDepth(depth);
	
	// å���� ������� �� ����� ���� �ٽ� ����ߴ�.
	output.FrameOutput = float4(
		1.0 - saturate(depth / 75.0),
		1.0 - saturate(depth / 125.0),
		1.0 - saturate(depth / 200.0),
		1.0);

	// �ᱹ
	// albedo�� ȭ���� uv�� �°� ������ �����ϰ�
	// normal�� ���� ���͸� �����ϰ�
	// depth�� ���� ���� ������ ���̴�.
	// ��, ��� ��Ҹ� ���� �ؽ��Ŀ� ����Ѵٰ� ���� �ȴ�.
	// �׸��� �̸� �̿��� light�� ó���ϴ� ���̴�.

	return output;
}