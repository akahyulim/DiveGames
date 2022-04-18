#pragma once
#include "DiveEngine.h"

// 생각해보니 굳이 Camera를 상속할 필요가 없다??
class SceneViewCamera
{
public:
	SceneViewCamera(const DirectX::XMFLOAT3& pos = { 0.0f, 5.0f, -10.0f });
	~SceneViewCamera() = default;

	void Update(float elapsedTime);

	void MouseRotate(const DirectX::XMFLOAT2& delta);

	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;
	DirectX::XMMATRIX GetViewProjectionMatrix() const;

	DirectX::XMVECTOR GetRotationQuaternion() const;
	DirectX::XMVECTOR GetForwardDirection() const;
	DirectX::XMVECTOR GetUpDirection() const;

	// 헤이즐의 경우 distance와 focal position을 이용해 position을 계산한다.
	DirectX::XMFLOAT3 GetPosition() const { return m_Position; }
	void SetPosition(const DirectX::XMFLOAT3& pos) { m_Position = pos; }

	void SetViewportSize(float width, float height);

	// 유니티 설정
	// fov
	// clipping plans: near, far
	// Camera Speed

private:
	void updateView();
	void updateProjection();

private:
	float m_Fov = 45.0f;	
	float m_AspectRatio = 1.778f;
	float m_NearPlane = 0.1f;
	float m_FarPlane = 1000.0f;

	DirectX::XMFLOAT3 m_Position = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);

	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_ProjMatrix;

	float m_Pitch	= 0.0f;
	float m_Yaw		= 0.0f;

	float m_RotateSpeed = 0.8f;

	// Viewport 크기에 맞춰 AspetRatio 계산을 위해 사용한다.
	float m_ViewportWidth = 0.0f;
	float m_ViewportHeight = 0.0f;
};