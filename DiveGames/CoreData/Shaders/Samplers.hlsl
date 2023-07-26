#ifdef COMPILEPS

Texture2D AlbedoMap : register(t0);
Texture2D MatalicMap: register(t1);
Texture2D NormalMap : register(t2);
Texture2D OcclusionMap: register(t3);

Texture2D<float> DepthTex : register(t4);
Texture2D<float4> ColorSpecIntTex : register(t5);
Texture2D<float3> NormalTex : register(t6);
Texture2D<float4> SpecPowTex : register(t7);

// 역시 s0 ~ s15까지
SamplerState AlbedoMapSampler : register(s0);
SamplerState NormalMapSampler : register(s1);


#endif