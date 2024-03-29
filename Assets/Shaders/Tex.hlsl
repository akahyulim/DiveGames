#include "Common.hlsl"

struct VertexInput
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
};

PixelInput MainVS(VertexInput input)
{
    PixelInput output;
    
    input.position.w = 1.0f;
    output.position = mul(input.position, cbFrame.world);
    output.position = mul(output.position, mul(cbFrame.view, cbFrame.projection));
    output.normal = mul(input.normal, (float3x3)cbFrame.world);
    output.normal = normalize(output.normal);
    output.texCoord = input.texCoord;
    
    return output;
}

float4 MainPS(PixelInput input) : SV_TARGET
{
    float4 diff;
    
    if(!has_texture_diffse())
        diff = cbMaterial.color;
    else
        diff = DiffuseMap.Sample(DiffuseMapSampler, input.texCoord);
    
    return diff;
}