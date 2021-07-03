/*
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};
*/
cbuffer BufferFrameGPU
{
	// float4x4�� matrix�� ���̰� ����?
	float4x4 world;
	float4x4 wvp;
};
/**/
struct VertexInput
{
	float4 position : POSITION0;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL0;
	float3 tangent	: TANGENT0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: TEXCOORD1;
	float3 tangent	: TEXCOORD2;
};

PixelInput mainVS(VertexInput input)
{
	PixelInput output;

	input.position.w = 1.0f;
	output.position = mul(input.position, wvp);

	output.uv = input.uv;
	output.normal = mul(input.normal, (float3x3)world).xyz;
	output.tangent = mul(input.tangent, (float3x3)world).xyz;

	return output;
}

// �������� ��� RenderTarget�� �޶������ �ʳ�?
// ���� �׳� ���δ�.
float4 mainPS(PixelInput input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}