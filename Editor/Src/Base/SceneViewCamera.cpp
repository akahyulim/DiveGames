#include "SceneViewCamera.h"

// 1. position 직접 이동이 맞는 것일까?
// 2. ScenePanel이 focus 중일 때만 입력을 처리하는 것이 맞다.
// 3. 아직 기타 함수를 만들지 않았다.
SceneViewCamera::SceneViewCamera()
{
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, DirectX::XMMatrixIdentity());
}

void SceneViewCamera::Update(float elapsedTime)
{
	// 입력 처리: Scene Panel이 선택되었을 때만!
	{
		{
			auto wheelDelta = Dive::Input::GetMouseWheelDelta();
			m_Position.z += wheelDelta;
		}

		if (Dive::Input::KeyPress(VK_MENU))//VK_MBUTTON))
		{
			// 상, 하, 좌, 우 = 이동
			auto delta = Dive::Input::GetMouseDelta();
			// 이건 잘못됐다??
			m_Position.x += delta.x;
			m_Position.y += delta.y;
		}

		if (Dive::Input::KeyPress(VK_RBUTTON))
		{
			auto delta = Dive::Input::GetMouseDelta();
			delta.x *= 0.003f;
			delta.y *= 0.003f;
			MouseRotate(delta);
		}
	}

	updateView();
	// 원래는 proj 값을 변경할 때 호출
	updateProjection();
}

void SceneViewCamera::MouseRotate(const DirectX::XMFLOAT2& delta)
{
	m_Yaw += delta.x * m_RotateSpeed;
	m_Pitch += delta.y * m_RotateSpeed;
}

DirectX::XMMATRIX SceneViewCamera::GetViewMatrix() const
{
	return DirectX::XMLoadFloat4x4(&m_ViewMatrix);
}

DirectX::XMMATRIX SceneViewCamera::GetProjectionMatrix() const
{
	return DirectX::XMLoadFloat4x4(&m_ProjMatrix);
}

DirectX::XMMATRIX SceneViewCamera::GetViewProjectionMatrix() const
{
	return GetViewMatrix() * GetProjectionMatrix();
}

DirectX::XMVECTOR SceneViewCamera::GetRotationQuaternion() const
{
	return DirectX::XMQuaternionRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f);
}

DirectX::XMVECTOR SceneViewCamera::GetForwardDirection() const
{
	auto matRot = DirectX::XMMatrixRotationQuaternion(GetRotationQuaternion());
	return DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f), matRot);
}

DirectX::XMVECTOR SceneViewCamera::GetUpDirection() const
{
	auto matRot = DirectX::XMMatrixRotationQuaternion(GetRotationQuaternion());
	return DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), matRot);
}

void SceneViewCamera::SetViewportSize(float width, float height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	updateProjection();
}

void SceneViewCamera::updateView()
{
	auto vecPos = DirectX::XMLoadFloat3(&m_Position);

	auto focusPosition = DirectX::XMVectorAdd(GetForwardDirection(), vecPos);
	
	auto view = DirectX::XMMatrixLookAtLH(vecPos, focusPosition, GetUpDirection());
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, view);
}

// Viewport 크기가 변경되었을 때 호출된다...?
void SceneViewCamera::updateProjection()
{
	float fieldOfView = 3.141592654f / 4.0f;
	//float screenAspect = m_ViewportWidth / m_ViewportHeight;

	auto proj = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane);
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, proj);
}
