// �������� ����ϴ� �͵� 

// GBuffer Texture
// ������ ������ �޳�...? �̰� srv ����.
Texture2D<float> DepthTexture : register(t0);
Texture2D<float4> ColorSpecIntTexture : register(t1);
Texture2D<float3> NormalTexture : register(t2);
Texture2D<float4> SpecPowTexture : register(t3);