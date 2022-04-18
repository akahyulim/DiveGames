#include "common.hlsl"

struct Pixel_Input
{
	float4 position : SV_POSITION;
	float2 texCoord	: TEXCOORD0;
};

struct Pixel_Output
{
	float4 albedo : SV_TARGET0;
};

Pixel_Input mainVS(Vertex_PosTex input)
{
	Pixel_Input output;
	
	input.position.w = 1.0f;

	output.position = mul(input.position, g_sprite_world);
	output.position = mul(output.position, g_view);
	output.position = mul(output.position, g_proj);

	output.texCoord = input.texCoord;
	if (IsSpriteFlipX())
	{
		output.texCoord.x = 1.0f - output.texCoord.x;
	}
	if (IsSpriteFlipY())
	{
		output.texCoord.y = 1.0f - output.texCoord.y;
	}

	return output;
}

// 추후 common_sampler로 이동
SamplerState linearSampler;

Pixel_Output mainPS(Pixel_Input input)
{
	Pixel_Output output;

	if (!HasSpriteTexture())
	{
		output.albedo = float4(0.0f, 0.0f, 0.0f, 0.0f);
		return output;
	}

	float4 color = g_sprite_color;
	color *= material_Albedo.Sample(linearSampler, input.texCoord);

	output.albedo = color;

	return output;
}