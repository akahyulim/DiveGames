#include "EditorCamera.h"

EditorCamera::EditorCamera(float fov, float aspectRatio, float nearPlane, float farPlane)
	: m_Fov(fov), m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane)
{
	updateView();
}

void EditorCamera::Update(float elapsedTime)
{
	// Hazle�� �� ������ �Է��� ó���Ѵ�.

	// �� �� position�� view matrix�� ����Ѵ�.
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
	// ��ġ, ��溤��, lookAt
}

// Viewport ũ�Ⱑ ����Ǿ��� �� ȣ��ȴ�...?
void EditorCamera::updateProjection()
{
	// fov, aspectRatio, near & far plane
}
