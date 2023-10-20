#include "Common.hlsl"

Pixel_PosTexNorTan MainVS(Vertex_Skinned input)
{
	Pixel_PosTexNorTan output;
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
	
	output.normal = mul(input.normal, (float3x3)cbWorldMatrixVS);
	output.normal = normalize(output.normal);
	
	output.tangent = mul(input.tangent, (float3x3)cbWorldMatrixVS);
	output.tangent = normalize(output.tangent);

	return output;
}

float4 MainPS(Pixel_PosTexNorTan input) : SV_TARGET0
{
	return DiffuseMap.Sample(DiffuseMapSampler, cbMaterialOffsetPS + input.tex * cbMaterialTilingPS);
}