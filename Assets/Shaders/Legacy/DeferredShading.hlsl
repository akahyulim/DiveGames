//#include "Common.hlsl"

struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal	: NORMAL;
	float2 UV		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 UV		: TEXCOORD0;
	float3 Normal	: TEXCOORD1;
};

// �⺻ ��ȯ
VS_OUTPUT mainVS(VS_INPUT input)
{
	VS_OUTPUT output;

	return output;
}

struct PS_OUTPUT
{
	float4 ColorSpecInt : SV_TARGET0;
	float4 Normal		: SV_TARGET1;
	float4 SpecPow		: SV_TARGET2;
};

// GBuffer���ٰ� �׸���.
PS_OUTPUT mainPS(VS_OUTPUT input)
{
	PS_OUTPUT output;
	output.ColorSpecInt = float4(1.0f, 0.0f, 0.0f, 1.0f);
	output.Normal = float4(0.0f, 1.0f, 0.0f, 1.0f);
	output.SpecPow = float4(0.0f, 0.0f, 1.0f, 1.0f);

	return output;
}

// pixel shader
// ������ RT�� �ش��ϴ� Data ����
// ���� Light Shader���� SRV�� ���ø��Ѵ�.