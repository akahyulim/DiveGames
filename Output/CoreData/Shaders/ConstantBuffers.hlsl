
// ���� �տ� c�� ���̴� ���� ���� �� ����.
// �׸��� vs�� ps�� �������� ���� ��� �ڿ� ps�� �߰��� ���̱⵵ �ߴ�.

#ifdef COMPILEVS

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// FrameVS
// CameraVS : ���⿡ cViewProj�� view�� proj�� ���� �����ϴ� �� �ϴ�.
// LightVS
// ObjectVS : ���⿡ worldTransform�� cModel�̸����� ���޵ȴ�.

#endif

#ifdef COMPILEPS

// FramePS
// CameraPS
// LightPS
// MaterialPS

#endif