#pragma once
#include "DiveEngine.h"

// RenderComponent도, Input도 없기 때문에
// 구현을 나중으로 미룬다.
class EditorCamera : public Dive::Camera
{
public:
	EditorCamera() = default;
	EditorCamera(float fov, float aspectRatio, float nearPlane, float farPlane);
	~EditorCamera() = default;

	void Update(float elapsedTime);

	const DirectX::XMFLOAT4X4& GetViewMatrix() const { return m_ViewMatrix; }
	DirectX::XMFLOAT4X4 GetViewProjectionMatrix() const;

private:
	void updateView();
	void updateProjection();

private:
	float m_Fov = 45.0f;
	float m_AspectRatio = 1.778f;
	float m_NearPlane = 0.1f;
	float m_FarPlane = 1000.0f;

	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT3 m_Position;

	float m_ViewWidth;
	float m_ViewHeight;

};