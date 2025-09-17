#include "Constants.hlsli"

static const float3 arrBasePos[4] =
{
    float3(-1.0, 0.0, 1.0),
    float3(1.0, 0.0, 1.0),
    float3(-1.0, 0.0, -1.0),
    float3(1.0, 0.0, -1.0),
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};

PSInput MainVS(uint VertexID : SV_VertexID)
{
    PSInput output;
    float3 pos = arrBasePos[VertexID] * 100.0;
    output.worldPos = pos;
    output.position = mul(float4(pos, 1.0f), cbCameraVS.viewMatrix);
    output.position = mul(output.position, cbCameraVS.projMatrix);
    
    return output;
}


