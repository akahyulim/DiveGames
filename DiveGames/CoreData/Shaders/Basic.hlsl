#include "Common.hlsl"

Pixel_PosCol MainVS(Vertex_PosCol input)
{
	Pixel_PosCol output;
	
	input.position.w = 1.0f;
	output.position = mul(input.position, cbWorldMatrixVS);
	output.position = mul(output.position, cbViewMatrixVS);
	output.position = mul(output.position, projMatrix);

	output.color = input.color;

	return output;
}

float4 MainPS(Pixel_PosCol) : VS_TARGET
{
	float4 input.color;
}