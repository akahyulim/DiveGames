/*
	����: ������( akahyulim@gmail.com )
*/

//= INCLUDES ===============================
#include "Common.hlsl"
//==========================================

Pixel_PosCol mainVS(Vertex_PosCol input)
{
	Pixel_PosCol output = (Pixel_PosCol)0;

	input.position.w = 1.0f;

	//output.position = mul(input.position, world);	// world�� ���� ���޹޾ƾ� �Ѵ�.
	output.position = mul(output.position, g_view);
	output.position = mul(output.position, g_proj);
	output.color = input.color;

	return output;
}

float4 mainPS(Pixel_PosCol input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 0.0f, 0.0f);//input.color;
}