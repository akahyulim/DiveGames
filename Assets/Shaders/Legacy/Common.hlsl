// 공통으로 사용하는 것들 

// GBuffer Texture
// 순서에 영향을 받나...? 이건 srv 같다.
// 어떻게 Common으로 bind했는지 모르겠다.
Texture2D<float> DepthTexture : register(t0);
Texture2D<float4> ColorSpecIntTexture : register(t1);
Texture2D<float3> NormalTexture : register(t2);
Texture2D<float4> SpecPowTexture : register(t3);

// constants

// values
static const float2 g_SpecPowerRange = { 10.0, 250.0 };

// structs

// functions