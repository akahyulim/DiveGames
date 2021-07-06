#include "Camera.h"


namespace dive
{
	Camera::Camera(GameObject* owner)
		: Component(typeid(Camera).hash_code(), owner)
	{
	}
	
	Camera::~Camera()
	{
	}

	void Camera::Update(float deltaTime)
	{
		// 이전 위치와 현재 위치를 비교하여 
		// dirty check를 한다.
	}

	DirectX::XMMATRIX Camera::computeViewMatrix() const
	{
		// 위치
		// 바라보는 방향 = 회전 사원수 * 전방 벡터
		// 위쪽 방향 = 회전 사원수 * 상향 벡터

		// 바라보는 방향 += 위치

		// 위 세가지 값을 이용해 LookAtLH 계산

		return DirectX::XMMATRIX();
	}
}