
// ���� �տ� c�� ���̴� ���� ���� �� ����.
// �׸��� vs�� ps�� �������� ���� ��� �ڿ� ps�� �߰��� ���̱⵵ �ߴ�.
// ũ�⸦ 4����Ʈ�� ������ �ʾƵ� �ǳ�? urho�� ���� ���̸� ���� �ʾҴ�.

#ifdef COMPILEVS

// FrameVS

// CameraVS : ���⿡ cViewProj�� view�� proj�� ���� �����ϴ� �� �ϴ�.
cbuffer CameraVS : register(b0)
{
//	float3 cameraPos;
	matrix viewMatrix;
	matrix projectionMatrix;

}
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
cbuffer CameraPS : register(b0)
{
	float3 cameraPos;	// �̸��� ������ �ȵȴ�.
}

// LightPS
cbuffer LightPS : register(b1)
{
	float4 lightColor;
	float4 lightDir;
	float lightRange;
	float lightSpotAngle;
	//float lightIntensity;	// �̰� material Ư���� �� �ϴ�.
}
// MaterialPS

#endif