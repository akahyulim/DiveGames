#include "ConstantBuffers.hlsl"
#include "Samplers.hlsl"

// �̰� Ŭ�� �����̽���.( -1.0 ~ 1.0 )
static const float2 arrBasePos[4] = 
{
	float2(-1.0, 1.0),		// �»�
	float2(1.0, 1.0),		// ���
	float2(-1.0, -1.0),		// ����
	float2(1.0, -1.0),		// ����
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 cpPos : TEXCOORD0;
};

VS_OUTPUT MainVS(uint VertexID : SV_VERTEXID)
{
	VS_OUTPUT output;

	output.position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);	// projection space
	output.cpPos = output.position.xy;								// clip space

	return output;
}

float4 MainPS(VS_OUTPUT input) : SV_TARGET
{
	// ����Ƶ� ��±��� �Ϸ�
	int3 location3 = int3(input.position.xy, 0);
	float4 diffMap = ColorSpecIntTex.Load(location3);

	return float4(diffMap.xyz, 1.0f);
}