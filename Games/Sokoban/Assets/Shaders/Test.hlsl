#include "Resources.hlsli"
#include "Constants.hlsli"

static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0),
	float2(1.0, 1.0),
    float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

struct VSToPS
{
    float4 position : SV_POSITION;
    float2 cpPos : TEXCOORD0;
};

VSToPS MainVS(uint VertexID : SV_VERTEXID)
{
    VSToPS output;

    output.position = float4(arrBasePos[VertexID].xy, 0.0, 1.0);
    output.cpPos = output.position.xy;

    return output;
}

float4 MainPS(VSToPS input) : SV_Target
{
    return float4(0.3, 0.3, 1.0, 1.0);
}