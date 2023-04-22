#ifdef COMPILEPS

// t0 ~ t15까지
// 그런데 종류가 다양하다.
Texture2D DiffMapTex : register(t0);
Texture2D NormalMapTex : register(t1);
// GBuffer는 어떻게 해야 할까..?
Texture2D<float> DepthTex : register(t2);
Texture2D<float4> ColorSpecIntTex : register(t3);
Texture2D<float3> NormalTex : register(t4);
Texture2D<float4> SpecPowTex : register(t5);

// 역시 s0 ~ s15까지
SamplerState DiffMapSampler : register(s0);
SamplerState NormalMapSampler : register(s1);


#endif