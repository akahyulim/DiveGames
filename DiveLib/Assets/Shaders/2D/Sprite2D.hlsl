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
    output.position = mul(pos, g_viewProjection);
    output.color = input.color;
    output.uv = input.uv;
    output.tiling = input.tiling;
    output.texIndex = input.texIndex;
    
    return output;
}

float4 SampleSprite(uint idx, float2 uv)
{
    switch (idx)
    {
        case 0: return g_spriteTex[0].Sample(g_clampPointSampler, uv);
        case 1: return g_spriteTex[1].Sample(g_clampPointSampler, uv);
        case 2: return g_spriteTex[2].Sample(g_clampPointSampler, uv);
        case 3: return g_spriteTex[3].Sample(g_clampPointSampler, uv);
        case 4: return g_spriteTex[4].Sample(g_clampPointSampler, uv);
        case 5: return g_spriteTex[5].Sample(g_clampPointSampler, uv);
        case 6: return g_spriteTex[6].Sample(g_clampPointSampler, uv);
        case 7: return g_spriteTex[7].Sample(g_clampPointSampler, uv);
        case 8: return g_spriteTex[8].Sample(g_clampPointSampler, uv);
        case 9: return g_spriteTex[9].Sample(g_clampPointSampler, uv);
        case 10: return g_spriteTex[10].Sample(g_clampPointSampler, uv);
        case 11: return g_spriteTex[11].Sample(g_clampPointSampler, uv);
        case 12: return g_spriteTex[12].Sample(g_clampPointSampler, uv);
        case 13: return g_spriteTex[13].Sample(g_clampPointSampler, uv);
        case 14: return g_spriteTex[14].Sample(g_clampPointSampler, uv);
        case 15: return g_spriteTex[15].Sample(g_clampPointSampler, uv);
        case 16: return g_spriteTex[16].Sample(g_clampPointSampler, uv);
        case 17: return g_spriteTex[17].Sample(g_clampPointSampler, uv);
        case 18: return g_spriteTex[18].Sample(g_clampPointSampler, uv);
        case 19: return g_spriteTex[19].Sample(g_clampPointSampler, uv);
        case 20: return g_spriteTex[20].Sample(g_clampPointSampler, uv);
        case 21: return g_spriteTex[21].Sample(g_clampPointSampler, uv);
        case 22: return g_spriteTex[22].Sample(g_clampPointSampler, uv);
        case 23: return g_spriteTex[23].Sample(g_clampPointSampler, uv);
        case 24: return g_spriteTex[24].Sample(g_clampPointSampler, uv);
        case 25: return g_spriteTex[25].Sample(g_clampPointSampler, uv);
        case 26: return g_spriteTex[26].Sample(g_clampPointSampler, uv);
        case 27: return g_spriteTex[27].Sample(g_clampPointSampler, uv);
        case 28: return g_spriteTex[28].Sample(g_clampPointSampler, uv);
        case 29: return g_spriteTex[29].Sample(g_clampPointSampler, uv);
        case 30: return g_spriteTex[30].Sample(g_clampPointSampler, uv);
        case 31: return g_spriteTex[31].Sample(g_clampPointSampler, uv);
        
        default:
            return g_spriteTex[0].Sample(g_clampPointSampler, uv);
    }
}

float4 MainPS(PS_INPUT input) : SV_Target
{
    uint texIdx = (uint) input.texIndex;
    float2 uv = input.uv * input.tiling;
    float4 texColor = SampleSprite(texIdx, uv);
    return texColor * input.color;
}