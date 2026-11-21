#include "Common.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float2 tiling : TEXCOORD1;
    float texIndex : TEXCOORD2;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float2 tiling : TEXCOORD1;
    float texIndex : TEXCOORD2;
};

PS_INPUT MainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 pos = float4(input.position, 1.0f);
    output.position = mul(pos, viewProjection);
    output.color = input.color;
    output.uv = input.uv;
    output.tiling = input.tiling;
    output.texIndex = input.texIndex;
    
    return output;
}

float4 MainPS(PS_INPUT input) : SV_Target
{
    float4 texColor = SpriteTex.Sample(ClampPointSampler, input.uv);
    return texColor * input.color;
}