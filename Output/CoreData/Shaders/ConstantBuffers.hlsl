
// 역시 앞에 c를 붙이는 편이 나을 것 같다.
// 그리고 vs와 ps의 변수명이 같을 경우 뒤에 ps를 추가로 붙이기도 했다.

#ifdef COMPILEVS

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// FrameVS
// CameraVS : 여기에 cViewProj로 view와 proj의 곱을 저장하는 듯 하다.
// LightVS
// ObjectVS : 여기에 worldTransform이 cModel이름으로 전달된다.

#endif

#ifdef COMPILEPS

// FramePS
// CameraPS
// LightPS
// MaterialPS

#endif