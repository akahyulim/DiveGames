#include "Common.hlsl"

cbuffer cbVertex : register(b2)
{
    matrix LightViewProj : packoffset(c0);
}

void MainVS(float4 iPos : POSITION0,
            out float4 oPos : SV_POSITION)
{
    iPos.w = 1.0f;
    
    oPos = mul(iPos, ObjectWorld);
    oPos = mul(oPos, LightViewProj);
}