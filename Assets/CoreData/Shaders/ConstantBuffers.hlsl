
// urho3d ����
// frame, camera, zone, light, material �� 5�� + objectVS�� ����
// �⺻������ vs�� ps�� ������ �ٸ�����, ���ų� �ߺ������ϴ� ��쵵 ����
// map/unmap�� batch���� ���� ��Ҹ��� ������Ʈ
// bind�� Graphics::SetShaders() ���ο��� �� ���� ����

#ifdef COMPILEVS

// FrameVS

// CameraVS : ���⿡ cViewProj�� view�� proj�� ���� �����ϴ� �� �ϴ�.
cbuffer CameraVS : register(b0)
{
//	float3 cbCameraPosPS;
	matrix cbViewMatrixVS;
	matrix cbProjMatrixVS;


}
// LightVS
// ObjectVS : ���⿡ worldTransform�� cModel�̸����� ���޵ȴ�.
cbuffer ObjectVS : register(b1)
{
	matrix cbWorldMatrixVS;
}

#endif

#ifdef COMPILEPS

// FramePS
// CameraPS
cbuffer CameraPS : register(b0)
{
	float3 cbCameraPosPS;	// �̸��� ������ �ȵȴ�.
	float cameraPoo;
	float4 cbPerspectiveValuePS;
	matrix cbViewInvPS;
}

// LightPS
cbuffer LightPS : register(b1)
{
	float3 cbLightPosPS;
	float cbLightRangePS;
	float3 cbLightColorPS;
	float cbLightSpotAnglePS;
	float3 cbLightDirPS;
	float lightPoo;
	//float lightIntensity;	// �̰� material Ư���� �� �ϴ�.
}

// MaterialPS
cbuffer MaterialPS : register(b2)
{
	float4 cbMaterialDiffColorPS;
}

#endif