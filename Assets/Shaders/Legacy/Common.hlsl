// �������� ����ϴ� �͵� 

// GBuffer Texture
// ������ ������ �޳�...? �̰� srv ����.
// ��� Common���� bind�ߴ��� �𸣰ڴ�.
Texture2D<float> DepthTexture : register(t0);
Texture2D<float4> ColorSpecIntTexture : register(t1);
Texture2D<float3> NormalTexture : register(t2);
Texture2D<float4> SpecPowTexture : register(t3);

// constants

// values
static const float2 g_SpecPowerRange = { 10.0, 250.0 };

// structs

// functions