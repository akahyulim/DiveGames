//#ifdef COMPILEPS

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap: register(t1);
Texture2D SpecularMap : register(t2);

Texture2D<float> DepthTex : register(t3);
Texture2D<float4> ColorSpecIntTex : register(t4);
Texture2D<float3> NormalTex : register(t5);
Texture2D<float4> SpecPowTex : register(t6);

// 역시 s0 ~ s15까지
SamplerState DiffuseMapSampler : register(s0);
SamplerState NormalMapSampler : register(s1);
SamplerState SpecularMapSampler : register(s2);


//#endif