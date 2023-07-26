#include "Common.hlsl"

Pixel_PosTex MainVS(Vertex_Skinned input)
{
	Pixel_PosTex output;
	input.position.w = 1.0f;
	
	for (int i = 0; i < 4; ++i)
	{
		if (input.boneIDs[i] == -1)
			continue;
		if (input.boneIDs[i] >= 100)
		{
			output.position = input.position;
			break;
		}

		float4 localPosition = mul(input.position, cbSkinMatrix[input.boneIDs[i]]);
		output.position += localPosition * input.boneWeights[i];
	}
	
	//output.position = mul(input.position, GetSkinMatrix(input.boneIDs, input.boneWeights));
	output.position = mul(input.position, cbWorldMatrixVS);
	//output.position = mul(output.position, cbWorldMatrixVS);	// root Transform�� �ߺ��ȴ�. => animation���� hierachy�� �����Ѵٸ� �̰� ����Ѵ�.
	output.position = mul(output.position, cbViewMatrixVS);
	output.position = mul(output.position, cbProjMatrixVS);

	output.tex = input.tex;

	return output;
}

float4 MainPS(Pixel_PosTex input) : SV_TARGET0
{
	return AlbedoMap.Sample(AlbedoMapSampler, input.tex);
}