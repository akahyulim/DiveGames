#include "common.hlsl"

// vertex shade
// input�� ���� �ؾ� ����?
// �̸�ó�� light�� �ؾ��ϳ�..?
// ������ ��κ� cb�� ���޹޴� ��?

Texture2D<float> DepthTexture		: register(t0);
Texture2D<float4> AlbedoTexture		: register(t1);
Texture2D<float4> NormalTexture		: register(t2);
Texture2D<float4> MaterialTexture	: register(t3);

struct Pixel_Output
{
	float4 FrameOutput : SV_TARGET0;
};

// pixel shade
// �ϴ� depth�� �������� ����Ѵ�.