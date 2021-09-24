#include "Common.hlsl"

Pixel_PosCol mainVS(Vertex_PosCol input)
{
	Pixel_PosCol output = (Pixel_PosCol)0;
	output.position		= mul(input.position, g_wvp);
	output.color		= input.color;

	return output;
}

float4 mainPS(Pixel_PosCol input) : SV_TARGET
{
	return input.color;
}