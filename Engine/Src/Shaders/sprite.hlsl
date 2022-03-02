
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float4 color	: COLOR;
	float2 tex		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 tex		: TEXCOORD0;
};

VS_OUTPUT mainVS(VS_INPUT input)
{
	VS_OUTPUT output;
	
	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.color = input.color;
	output.tex = input.tex;

	return output;
}

Texture2D shaderTexture;
SamplerState linearSampler;

float4 mainPS(VS_OUTPUT input) : SV_TARGET
{
	float4 texColor = shaderTexture.Sample(linearSampler, input.tex);
	return texColor * input.color;
}