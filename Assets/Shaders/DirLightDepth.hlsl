#include "Common.hlsl"

// ���⿡�� ���庯ȯ�� �ϰ�
// gs���� ������ viewProj��ȯ�� �ϴ� �� �´� �� ����.
float4 MainVS(float4 pos : POSITION0) : SV_Position
{
    pos.w = 1.0;
    return mul(pos, ObjectWorld);
}

cbuffer cbGeometry : register(b2)
{
    float4x4 CascadeViewProj[3] : packoffset(c0);
}

struct GS_OUTPUT
{
    float4 pos : SV_Position;
    uint rtIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void MainGS(triangle float4 pos[3] : SV_Position, inout TriangleStream<GS_OUTPUT> outStream)
{
    for (int face = 0; face < 3; face++)
    {
        GS_OUTPUT output;
        output.rtIndex = face;
        
        for (int v = 0; v < 3; v++)
        {
            output.pos = mul(pos[v], CascadeViewProj[face]);
            outStream.Append(output);
        }
        outStream.RestartStrip();
    }
}