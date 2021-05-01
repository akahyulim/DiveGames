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

// 색 cubuffer

float4 mainPS(PixelInput input) : SV_TARGET
{
	float4 color = fontAtlas.Sample(fontSampler, input.tex);
	
	// 색이 없다면 투명화 시켜 공백을 만든다.
	if (color.r == 0.0f)
	{
		color.a = 0.0f;
	}
	else
	{
		color.a = 1.0f;
		color.xyz *= float4(1.0f, 1.0f, 0.0f, 1.0f);
	}

	return color;
}