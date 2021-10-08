// 공통으로 사용하는 것들 

// GBuffer Texture
// 순서에 영향을 받나...? 이건 srv 같다.
Texture2D<float> DepthTexture : register(t0);
Texture2D<float4> ColorSpecIntTexture : register(t1);
Texture2D<float3> NormalTexture : register(t2);
Texture2D<float4> SpecPowTexture : register(t3);