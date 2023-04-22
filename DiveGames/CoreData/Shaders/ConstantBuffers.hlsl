
// 역시 앞에 c를 붙이는 편이 나을 것 같다.
// 그리고 vs와 ps의 변수명이 같을 경우 뒤에 ps를 추가로 붙이기도 했다.
// 크기를 4바이트로 맞추지 않아도 되나? urho는 따로 더미를 넣지 않았다.

#ifdef COMPILEVS

// FrameVS

// CameraVS : 여기에 cViewProj로 view와 proj의 곱을 저장하는 듯 하다.
cbuffer CameraVS : register(b0)
{
//	float3 cameraPos;
	matrix viewMatrix;
	matrix projectionMatrix;

}
// LightVS
// ObjectVS : 여기에 worldTransform이 cModel이름으로 전달된다.
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
	float3 cameraPos;	// 이름이 같으면 안된다.
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
	//float lightIntensity;	// 이건 material 특성인 듯 하다.
}

// MaterialPS
cbuffer MaterialPS : register(b2)
{
	float4 materialDiffColor;
}

#endif