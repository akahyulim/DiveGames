
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
	matrix worldMatrix;
}

#endif

#ifdef COMPILEPS

// FramePS
// CameraPS
cbuffer CameraPS : register(b0)
{
	float3 cameraPos;	// �̸��� ������ �ȵȴ�.
	float cameraPoo;
}

// LightPS
cbuffer LightPS : register(b1)
{
	float3 lightPos;
	float lightRange;
	float3 lightColor;
	float lightSpotAngle;
	float3 lightDir;
	float lightPoo;
	//float lightIntensity;	// �̰� material Ư���� �� �ϴ�.
}

// MaterialPS
cbuffer MaterialPS : register(b2)
{
	float4 materialDiffColor;
}

#endif