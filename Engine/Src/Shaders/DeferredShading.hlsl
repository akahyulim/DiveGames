#include "common.hlsl"

struct Pixel_Input
{
	float4 position : SV_POSITION;
	float2 texCoord	: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
};

Pixel_Input mainVS(Vertex_PosTexNorTan input)
{
	Pixel_Input output;

	input.position.w = 1.0f;

	output.position = mul(input.position, g_World);
	output.position = mul(output.position, g_View);
	output.position = mul(output.position, g_Proj);

	output.texCoord = (input.texCoord / g_MtrlTiling) + g_MtrlOffset;

	output.normal = mul(input.normal, (float3x3)g_World);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)g_World);
	output.tangent = normalize(output.tangent);

	return output;
}

struct Pixel_GBuffer_Out
{
	float4 Albedo	: SV_TARGET0;
	float4 Normal	: SV_TARGET1;
	float4 Material : SV_TARGET2;
};

Pixel_GBuffer_Out mainPS(Pixel_Input input)
{
	Pixel_GBuffer_Out output;

	float4 albedo = g_MtrlColor;
	if (HasAlbedoTexture())
	{
		// map이 있다면 material color는 무시하는 것이 맞겠지?
		// => 유니티의 경우 위에 적용됐다.
		albedo = material_Albedo.Sample(LinearSampler, input.texCoord);
		albedo.w = 1.0f;
	}

	// normal
	float3 normal = input.normal;
	if (HasNormalTexture())
	{
		// 일단 현재 sponza의 normal이 없다고 나온다.
		// 그리고 샘플링도 이렇게 하는 게 아닐 것이다.
	//	normal *= material_Normal.Sample(LinearSampler, input.texCoord);
	}

	output.Albedo	= albedo;
	output.Normal	= float4(normal * 0.5 + 0.5, 1.0f);
	//output.Material = float4(normal.xyz, 1.0f);

	return output;
}