#ifdef COMPILEPS

// t0 ~ t15����
// �׷��� ������ �پ��ϴ�.
Texture2D DiffMapTex : register(t0);
Texture2D NormalMapTex : register(t1);
// GBuffer�� ��� �ؾ� �ұ�..?
Texture2D<float> DepthTex : register(t2);
Texture2D<float4> ColorSpecIntTex : register(t3);
Texture2D<float3> NormalTex : register(t4);
Texture2D<float4> SpecPowTex : register(t5);

// ���� s0 ~ s15����
SamplerState DiffMapSampler : register(s0);
SamplerState NormalMapSampler : register(s1);


#endif