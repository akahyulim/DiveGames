#include "common.hlsl"

// ���� ���� ����
float4x4 matWorld;
float4x4 matView;
float4x4 matProj;

// �ؽ���
Texture2D texAlbedo;

// vertex shader
Pixel_PosTexNorTan mainVS(Vertex_PosTexNorTan input)
{
	Pixel_PosTexNorTan output = (Pixel_PosTexNorTan)0;

	output.position.w = 1.0f;
	output.position = mul(input.position, matWorld);
	output.position = mul(output.position, matView);
	output.position = mul(output.position, matProj);

	output.uv = input.uv;
	output.normal = normalize(input.normal);
	output.tangent = input.tangent;

	return output;
}

// pixel shader
float4 mainPS(Pixel_PosTexNorTan input) : SV_Target
{
	return texAlbedo.Sample(samplerLinear, input.uv);
}

// technique11�̴�.
technique11 Base
{
	// pass
	pass p0
	{
		// state - SetXXXState()�� ����
		SetDepthStencilState(DepthEnabled, 0);
		SetBlendState(BlendDisabled, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullBackSolid);

		// shader - XXSetShader()�� ����
		SetVertexShader(CompileShader(vs_5_0, mainVS()));
		SetPixelShader(CompileShader(ps_5_0, mainPS()));
	}
}