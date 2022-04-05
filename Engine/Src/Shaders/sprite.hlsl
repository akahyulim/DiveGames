#include "common.hlsl"

struct Pixel_Input
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texCoord	: TEXCOORD0;
};

Pixel_Input mainVS(Vertex_PosColTex input)
{
	Pixel_Input output;
	
	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.color = input.color;
	output.texCoord = input.texCoord;

	return output;
}

Texture2D shaderTexture;
SamplerState linearSampler;

float4 mainPS(Pixel_Input input) : SV_TARGET
{
	float4 texColor = shaderTexture.Sample(linearSampler, input.texCoord);
	return texColor * input.color;
}