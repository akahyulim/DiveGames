#include "Common.hlsl"

// �̰� Ŭ�� �����̽���.( -1.0 ~ 1.0 )
static const float2 arrBasePos[4] = {
	float2(-1.0, 1.0),		// �»�
	float2(1.0, 1.0),		// ���
	float2(-1.0, -1.0),		// ����
	float2(1.0, -1.0),		// ����
};

/////////////////////////////////////////////////////////////////////////////
// Vertex shader
/////////////////////////////////////////////////////////////////////////////
struct VS_OUTPUT
{
	// å������ ���� position, uv��.
	float4 Position : SV_Position; // vertex position 
	float2 cpPos	: TEXCOORD0;
};

VS_OUTPUT mainVS(uint VertexID : SV_VertexID)
{
	VS_OUTPUT Output;

	// 0 ~ 6. ��, �ﰢ�� �� ��?
	// x, y�� ���� ����. 
	Output.Position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);	// projection space
	Output.cpPos = Output.Position.xy;								// clip space

	// ������ �������� projection ����� clip space�� �Ѵ�...
	// ��, ���� ������ 4������ 2�������� �̷������ �ϴ�.

	// ����ư rastrize �� ���� �޶�����.

	return Output;
}

// diffuse texture and sampler
Texture2D DiffuseTexture	: register(t0);
SamplerState LinearSampler	: register(s0);

float4 mainPS(VS_OUTPUT In) : SV_TARGET
{
	// ��ǥ�谡 �ٸ���?
	// ����������.
	float3 diffuse = DiffuseTexture.Sample(LinearSampler, In.cpPos);

	return float4(diffuse, 1.0f);
}