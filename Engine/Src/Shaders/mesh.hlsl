
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
};

VS_OUTPUT mainVS(VS_INPUT input)
{
	VS_OUTPUT output;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)world);
	output.tangent = normalize(output.tangent);

	return output;
}

float4 mainPS(VS_OUTPUT input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}