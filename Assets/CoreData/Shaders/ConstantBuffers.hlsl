
// urho3d 기준
// frame, camera, zone, light, material 총 5개 + objectVS로 구성
// 기본적으로 vs와 ps의 구성이 다르지만, 같거나 중복전달하는 경우도 존재
// map/unmap은 batch에서 개별 요소마다 업데이트
// bind는 Graphics::SetShaders() 내부에서 한 번에 실행

#ifdef COMPILEVS

// FrameVS

// CameraVS : 여기에 cViewProj로 view와 proj의 곱을 저장하는 듯 하다.
cbuffer CameraVS : register(b0)
{
//	float3 cbCameraPosPS;
	matrix cbViewMatrixVS;
	matrix cbProjMatrixVS;


}
// LightVS
// ObjectVS : 여기에 worldTransform이 cModel이름으로 전달된다.
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
	float3 cbCameraPosPS;	// 이름이 같으면 안된다.
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
	//float lightIntensity;	// 이건 material 특성인 듯 하다.
}

// MaterialPS
cbuffer MaterialPS : register(b2)
{
	float4 cbMaterialDiffColorPS;
}

#endif