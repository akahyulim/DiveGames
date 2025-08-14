#include "VSInput.hlsli"
#include "Constants.hlsli"

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

VSOutput MainVS(VSInput_Lit input)
{
    VSOutput output;

    float4 position = input.Position;
    position.w = 1.0f;
    
    output.Position = mul(position, cbMatrix.modelMatrix);
    output.WorldPos = output.Position.xyz;
    output.Position = mul(output.Position, cbMatrix.viewMatrix);
    output.Position = mul(output.Position, cbMatrix.projMatrix);

    output.Normal = mul(input.Normal, (float3x3)cbMatrix.modelMatrix);
    output.Normal = normalize(output.Normal);
    output.Tangent = mul(input.Tangent, (float3x3)cbMatrix.modelMatrix);
    output.Tangent = normalize(output.Tangent);

    output.UV = input.UV;
    
    return output;
}