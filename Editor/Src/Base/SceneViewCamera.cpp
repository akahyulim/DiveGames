#include "SceneViewCamera.h"

// 2. ScenePanel�� focus ���� ���� �Է��� ó���ϴ� ���� �´�.
// 3. ���� ��Ÿ �Լ��� ������ �ʾҴ�.
SceneViewCamera::SceneViewCamera()
{
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, DirectX::XMMatrixIdentity());
}

void SceneViewCamera::Update(float elapsedTime)
{
	// �Է� ó��: Scene Panel�� ���õǾ��� ����!
	{
		// �����Ӹ�ŭ ���� ���ͷ� �̵�
		auto wheelDelta = Dive::Input::GetMouseWheelDelta();
		if (wheelDelta != 0.0f)
		{
			DirectX::XMFLOAT3 focus;
			DirectX::XMStoreFloat3(&focus, GetForwardDirection());

			m_Position.x += wheelDelta * focus.x;
			m_Position.y += wheelDelta * focus.y;
			m_Position.z += wheelDelta * focus.z;
		}

		// ��, ��, ��, �� �̵�
		if (Dive::Input::KeyPress(VK_MENU))//VK_MBUTTON))
		{
			auto delta = Dive::Input::GetMouseDelta();
			delta.x *= 0.1f;
			delta.y *= -0.1f;
			
			DirectX::XMFLOAT3 up;
			DirectX::XMStoreFloat3(&up, GetUpDirection());
			DirectX::XMFLOAT3 right;
			DirectX::XMStoreFloat3(&right, DirectX::XMVector3Cross(GetUpDirection(), GetForwardDirection()));

			right.x *= delta.x;
			right.y *= delta.x;
			right.z *= delta.x;

			up.x *= delta.y;
			up.y *= delta.y;
			up.z *= delta.y;

			m_Position.x += right.x + up.x;
			m_Position.y += right.y + up.y;
			m_Position.z += right.z + up.z;
		}
		
		// ȸ��
		if (Dive::Input::KeyPress(VK_RBUTTON))
		{
			auto delta = Dive::Input::GetMouseDelta();
			delta.x *= 0.003f;
			delta.y *= 0.003f;
			MouseRotate(delta);
		}
	}

	updateView();
	// ������ proj ���� ������ �� ȣ��
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

// Viewport ũ�Ⱑ ����Ǿ��� �� ȣ��ȴ�...?
void SceneViewCamera::updateProjection()
{
	float fieldOfView = 3.141592654f / 4.0f;
	//float screenAspect = m_ViewportWidth / m_ViewportHeight;

	auto proj = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, m_AspectRatio, m_NearPlane, m_FarPlane);
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, proj);
}
