
// ���� �տ� c�� ���̴� ���� ���� �� ����.
// �׸��� vs�� ps�� �������� ���� ��� �ڿ� ps�� �߰��� ���̱⵵ �ߴ�.

#ifdef COMPILEVS

cbuffer MatrixBuffer : register(b0)
{
	//matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// FrameVS
// CameraVS : ���⿡ cViewProj�� view�� proj�� ���� �����ϴ� �� �ϴ�.
// LightVS
// ObjectVS : ���⿡ worldTransform�� cModel�̸����� ���޵ȴ�.
cbuffer ObjectVS : register(b1)
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