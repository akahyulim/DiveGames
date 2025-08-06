#include "Common.hlsl"

// ���⿡�� ���庯ȯ�� �ϰ�
// gs���� ������ viewProj��ȯ�� �ϴ� �� �´� �� ����.
float4 MainVS(float4 pos : POSITION) : SV_Position
{
    pos.w = 1.0;
    return mul(pos, ObjectWorld);
}

cbuffer cbGeometry : register(b2)
{
    float4x4 CubeViewProj[6] : packoffset(c0);
}

struct GS_OUTPUT
{
    float4 pos : SV_Position;
    uint rtIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void MainGS(triangle float4 inPos[3] : SV_Position, inout TriangleStream<GS_OUTPUT> outStream)
{
    for (int face = 0; face < 6; face++)
    {
        GS_OUTPUT output;
        output.rtIndex = face;
        
        for (int v = 0; v < 3; v++)
        {
            output.pos = mul(inPos[v], CubeViewProj[face]);
            outStream.Append(output);
        }
        outStream.RestartStrip();
    }
}