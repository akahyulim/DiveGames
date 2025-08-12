#include "Constants.hlsli"
#include "Resources.hlsli"

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
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