#include "VSInput.hlsli"
#include "Constants.hlsli"
#include "Resources.hlsli"

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

VSOutput MainVS(VSInput_Unlit input)
{
    VSOutput output;

    float4 position = input.Position;
    position.w = 1.0f;
    
    output.Position = mul(position, cb_Matrix.modelMatrix);
    output.Position = mul(output.Position, cb_Matrix.viewMatrix);
    output.Position = mul(output.Position, cb_Matrix.projMatrix);

    output.UV = input.UV;
    
    return output;
}