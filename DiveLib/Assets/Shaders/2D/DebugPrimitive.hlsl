#include "Common.hlsli"

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT MainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 pos = float4(input.position, 1.0f);
    output.position = mul(pos, viewProjection);    
    output.color = input.color;
    
    return output;
}

float4 MainPS(PS_INPUT input) : SV_Target
{
    return input.color;
}