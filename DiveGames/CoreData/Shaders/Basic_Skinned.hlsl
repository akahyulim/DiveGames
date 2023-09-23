#include "Common.hlsl"
/*
Pixel_Test MainVS(Vertex_Skinned input)
{
	Pixel_Test output;
	input.position.w = 1.0f;

	output.position = mul(input.position, cbWorldMatrixVS);
	output.position = mul(output.position, cbViewMatrixVS);
	output.position = mul(output.position, cbProjMatrixVS);

	output.color = input.boneWeights;
	 
	//output.color = input.boneIDs;
	
	return output;
}

float4 MainPS(Pixel_Test input) : SV_TARGET0
{
	return input.color;
}
*/

Pixel_PosTex MainVS(Vertex_Skinned input)
{
	Pixel_PosTex output;
	input.position.w = 1.0f;
	
	for (int i = 0; i < 4; ++i)
	{
		if (input.boneIDs[i] >= 100)
			continue;

		float4 localPosition = mul(input.position, cbSkinMatrix[input.boneIDs[i]]);
		output.position += localPosition * input.boneWeights[i];
	}
	//output.position = mul(input.position, GetSkinMatrix(input.boneIDs, input.boneWeights));
	//output.position = mul(input.position, cbWorldMatrixVS);
	output.position = mul(output.position, cbWorldMatrixVS);	// rootBone을 전달받았다면 따로 계산해야 한다.
	output.position = mul(output.position, cbViewMatrixVS);
	output.position = mul(output.position, cbProjMatrixVS);

	output.tex = input.tex;

	return output;
}

float4 MainPS(Pixel_PosTex input) : SV_TARGET0
{
	return DiffuseMap.Sample(DiffuseMapSampler, cbMaterialOffsetPS + input.tex * cbMaterialTilingPS);
}