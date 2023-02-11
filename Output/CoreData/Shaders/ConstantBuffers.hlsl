
// ���� �տ� c�� ���̴� ���� ���� �� ����.
// �׸��� vs�� ps�� �������� ���� ��� �ڿ� ps�� �߰��� ���̱⵵ �ߴ�.

#ifdef COMPILEVS

// FrameVS
cbuffer FrameVS : register(b0)
{
	float3 cameraPos;
	float dummy;
	matrix viewMatrix;
	matrix projectionMatrix;

}
// CameraVS : ���⿡ cViewProj�� view�� proj�� ���� �����ϴ� �� �ϴ�.
// LightVS
// ObjectVS : ���⿡ worldTransform�� cModel�̸����� ���޵ȴ�.
cbuffer ObjectVS : register(b2)
{
	float4x4 cModel;
}

#endif

#ifdef COMPILEPS

// FramePS
// CameraPS
// LightPS
// MaterialPS

#endif