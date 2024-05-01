#include "Common.hlsl"

struct VS_INPUT
{
    float4 position : POSITION0;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 bitangent : BINORMAL0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 bitangent : BINORMAL0;
};

VS_OUTPUT MainVS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    input.position.w = 1.0f;
    output.position = mul(input.position, cbFrameVertex.world);
    output.worldPos = output.position.xyz;
    output.position = mul(output.position, mul(cbFrameVertex.view, cbFrameVertex.projection));
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) cbFrameVertex.world);
    output.normal = normalize(output.normal);
    output.tangent = mul(input.tangent, (float3x3) cbFrameVertex.world);
    output.tangent = normalize(output.tangent);
    output.bitangent = mul(input.bitangent, (float3x3) cbFrameVertex.world);
    output.bitangent = normalize(output.bitangent);
    
    return output;
}

struct PS_GBUFFER
{
	float4 colorSpecIntensity : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 specPower : SV_TARGET2;
};

PS_GBUFFER MainPS(VS_OUTPUT input)
{
	PS_GBUFFER output;

    // diffuse
    float4 diff;
    if (!HasDiffuseTexture())
        diff = cbMaterialPixel.color;
    else
        diff = DiffuseMap.Sample(DiffuseMapSampler, input.tex);
    diff *= diff; // linear space
	output.colorSpecIntensity.xyz = diff.xyz;

    // normal
    float3 normal = input.normal;
    if (HasNormalTexture())
    {
        float4 bumpMap = NormalMap.Sample(NormalMapSampler, input.tex);
        bumpMap = (bumpMap * 2.0f) - 1.0f;

        normal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.bitangent) + (bumpMap.z * input.normal));
    }
	output.normal = float4(normal * 0.5f + 0.5f, 0.0);

    return output;
}