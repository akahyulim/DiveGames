
Texture2D DiffuseMap : register(t0);
Texture2D NormalMap: register(t1);
Texture2D SpecularMap : register(t2);

Texture2D<float> DepthTex : register(t3);
Texture2D<float4> DiffuseTex : register(t4);
Texture2D<float3> NormalTex : register(t5);
Texture2D<float4> SpecularTex : register(t6);

Texture2D<float> SpotShadowMap : register(t7);

SamplerState BaseSampler : register(s0);
SamplerComparisonState PcfSampler : register(s1);
