#include "VSInput.hlsli"
#include "Constants.hlsli"
#include "Resources.hlsli"

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Normal : TEXCOORD0;
    float3 Tangent : TEXCOORD1;
    float2 UV : TEXCOORD2;
};

VSOutput MainVS(VSInput_Lit input)
{
    VSOutput output;

    float4 position = input.Position;
    position.w = 1.0f;
    
    output.Position = mul(position, cb_Matrix.modelMatrix);
    output.Position = mul(output.Position, cb_Matrix.viewMatrix);
    output.Position = mul(output.Position, cb_Matrix.projMatrix);

    output.UV = input.UV;

    float3x3 normalMatrix = (float3x3) cb_Matrix.modelMatrix;
    output.Normal = normalize(mul(input.Normal, normalMatrix));
    output.Tangent = normalize(mul(input.Tangent, normalMatrix));

    return output;
}

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 Normal : TEXCOORD0;
    float3 Tangent : TEXCOORD1;
    float2 UV : TEXCOORD2;
};

float4 MainPS(PSInput input) : SV_TARGET
{
    float4 outColor;
    
    if (HasDiffuseMap())
    {
        outColor = DiffuseMap.Sample(WrapLinearSampler, input.UV);
    }
    else
    {
        outColor = cb_Material.diffuseColor;
    }
    
    return outColor;
}