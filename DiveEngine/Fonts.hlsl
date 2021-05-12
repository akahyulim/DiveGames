cbuffer MatrixBuffer
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
	float4x4 projOrtho;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORED0;
};

PixelInput mainVS(VertexInput input)
{
	PixelInput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projOrtho);

	output.tex = input.tex;

	return output;
}

Texture2D fontAtlas;
SamplerState fontSampler;

// »ö cubuffer

float4 mainPS(PixelInput input) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	color.r = fontAtlas.Sample(fontSampler, input.tex);
	color.g = color.r;
	color.b = color.r;
	color.a = color.r;

	color *= float4(1.0f, 1.0f, 0.0f, 1.0f);

	return color;
}