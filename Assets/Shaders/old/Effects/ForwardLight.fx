#include "DirectionalLight.ps"

float4x4 matWorld;
float4x4 matView;
float4x4 matProj;

Pixel_PosTexNorTanWorld mainVS(Vertex_PosTexNorTan input)
{
	Pixel_PosTexNorTanWorld output;

	output.worldPos = mul(input.position, matWorld);
	output.position = mul(float4(output.worldPos, 1.0f), matView);
	output.position = mul(output.position, matProj);

	output.uv = input.uv;

	output.normal = mul(input.normal, (float3x3)matWorld);
	output.normal = normalize(output.normal);
	
	output.tangent = mul(input.tangent, (float3x3)matWorld);
	output.tangent = output.tangent;

	return output;
}

technique11 LightRender
{
	pass p0
	{
		SetDepthStencilState(DepthEnabled, 0);
		SetBlendState(BlendDisabled, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(CullBackSolid);

		SetVertexShader(CompileShader(vs_5_0, mainVS()));
		SetPixelShader(CompileShader(ps_5_0, DirectionalLightPS()));
	}
}