#include "Common.hlsl"

Pixel_PosTex MainVS(Vertex_PosTexNorTan input)
{
	Pixel_PosTex output;
	
	input.position.w = 1.0f;
	output.position = mul(input.position, cbWorldMatrixVS);
	output.position = mul(output.position, cbViewMatrixVS);
	output.position = mul(output.position, cbProjMatrixVS);

	output.tex = input.tex;

	return output;
}

float4 MainPS(Pixel_PosTex input) : SV_TARGET0
{
	return AlbedoMap.Sample(AlbedoMapSampler, cbMaterialOffsetPS + input.tex * cbMaterialTilingPS);
}