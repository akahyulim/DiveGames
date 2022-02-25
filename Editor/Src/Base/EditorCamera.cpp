#include "EditorCamera.h"

EditorCamera::EditorCamera(float fov, float aspectRatio, float nearPlane, float farPlane)
	: m_Fov(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane)
{
	updateView();
}

void EditorCamera::Update(float elapsedTime)
{
	// Hazle은 이 곳에서 입력을 처리한다.

	// 그 후 position과 view matrix를 계산한다.
}

DirectX::XMFLOAT4X4 EditorCamera::GetViewProjectionMatrix() const
{
	auto matViewMatrix = DirectX::XMLoadFloat4x4(&m_ViewMatrix);
	auto matProjection = DirectX::XMLoadFloat4x4(&m_Projection);

	DirectX::XMFLOAT4X4 viewProjectionMatrix;
	DirectX::XMStoreFloat4x4(&viewProjectionMatrix, DirectX::XMMatrixMultiply(matViewMatrix, matProjection));

	return viewProjectionMatrix;
}

void EditorCamera::updateView()
{
	// 위치, 상방벡터, lookAt
}

// Viewport 크기가 변경되었을 때 호출된다...?
void EditorCamera::updateProjection()
{
	// fov, aspectRatio, near & far plane
}
