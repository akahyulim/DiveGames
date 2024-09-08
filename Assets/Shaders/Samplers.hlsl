// 텍스쳐의 경우 셰이더마다 선언해주는 편이 나을 듯 하다.

// material
Texture2D DiffuseMap : register(t0);
Texture2D NormalMap: register(t1);
Texture2D SpecularMap : register(t2);

// gbuffer
Texture2D<float> DepthTex : register(t3);
Texture2D<float4> DiffuseTex : register(t4);
Texture2D<float3> NormalTex : register(t5);
Texture2D<float4> SpecularTex : register(t6);

// shadow
Texture2D<float> SpotShadowMap : register(t7);
Texture2DArray<float> CascadeShadowMap : register(t8);

// samplers
SamplerState BaseSampler : register(s0);
SamplerComparisonState PcfSampler : register(s1);
