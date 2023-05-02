#include "Common.hlsl"

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 worldPos : TEXCOORD1;		// �̰� viewDir�� �ٲ� �� VS���� ���. �׷����� VSConstnat�� CameraPos�� �־�� �Ѵ�.
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

PixelInput MainVS(Vertex_PosTexNorTan input)
{
	PixelInput output;

	input.position.w = 1.0f;
	output.position = mul(input.position, cbWorldMatrixVS);
	output.position = mul(output.position, cbViewMatrixVS);
	output.position = mul(output.position, cbProjMatrixVS);

	output.worldPos = mul(input.position, cbWorldMatrixVS).xyz;

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)cbWorldMatrixVS);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)cbWorldMatrixVS);
	output.tangent = normalize(output.tangent);

	return output;
}