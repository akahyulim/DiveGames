cbuffer MatrixBuffer
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

PixelInput mainVS(VertexInput input)
{
	PixelInput output;

	input.position.w = 1.0f;
	
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.uv = input.uv;

	return output;
}

Texture2D shaderTex;
SamplerState samplerType;

float4 mainPS(PixelInput input) : SV_TARGET
{
	float4 color;
	color = shaderTex.Sample(samplerType, input.uv);
	return color;
}