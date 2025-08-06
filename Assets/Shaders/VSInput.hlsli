
struct VSInput_PN
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
};

struct VSInput_PNT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT0;
};

struct VSInput_Lit
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD0;
};