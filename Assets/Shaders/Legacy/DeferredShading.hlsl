#include "Common.hlsl"


// constant buffers
cbuffer cbPerObjectVS : register(b0)
{
	// offset의 단위는 어떻게 되는 것일까?
	float4x4 WorldViewProjection	: packoffset(c0);
	float4x4 World					: packoffset(c4);
}

cbuffer cbPerObjectPS : register(b0)
{
	float specExp		: packoffset(c0);
	float specIntensity	: packoffset(c0.y);
}

// diffuse texture and sampler
Texture2D DiffuseTexture	: register(t0);
SamplerState LinearSampler	: register(s0);

// shaders input / output
struct VS_INPUT
{
	float4 Position : POSITION;
	float2 UV		: TEXCOORD0;
	float3 Normal	: NORMAL0;
	float3 Tangent	: TANGENT0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 UV		: TEXCOORD0;
	float3 Normal	: TEXCOORD1;
	float3 Tangent	: TEXCOORD2;
};

// vertex shader
VS_OUTPUT mainVS(VS_INPUT input)
{
	VS_OUTPUT output;

	input.Position.w = 1.0f;
	output.Position = mul(input.Position, WorldViewProjection);

	output.UV		= input.UV;
	output.Normal	= mul(input.Normal, (float3x3)World);
	output.Tangent	= mul(input.Tangent, (float3x3)World);

	return output;
}

struct PS_OUTPUT
{
	float4 ColorSpecInt : SV_TARGET0;
	float4 Normal		: SV_TARGET1;
	float4 SpecPow		: SV_TARGET2;
};

PS_OUTPUT PackGBuffer(float3 BaseColor, float3 Normal, float SpecIntensity, float SpecPower)
{
	PS_OUTPUT output;

	float SpecPowerNorm = max(0.0001, (SpecPower - g_SpecPowerRange.x) / g_SpecPowerRange.y);

	output.ColorSpecInt = float4(BaseColor.rgb, SpecIntensity);
	output.Normal		= float4(Normal * 0.5 + 0.5, 0.0);
	output.SpecPow		= float4(SpecPowerNorm, 0.0, 0.0, 0.0);

	return output;
}

// pixel shader
PS_OUTPUT mainPS(VS_OUTPUT input)
{
	float3 DiffuseColor = DiffuseTexture.Sample(LinearSampler, input.UV);

	// 샘플링한 색을 제곱한 이유는 아직 모른다.
	DiffuseColor *= DiffuseColor;

	return PackGBuffer(DiffuseColor, normalize(input.Normal), specIntensity, specExp);
}