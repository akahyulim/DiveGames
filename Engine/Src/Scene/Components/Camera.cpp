#include "DivePch.h"
#include "Camera.h"
#include "Transform.h"
#include "../GameObject.h"
//#include "Core/FileStream.h"
#include "Core/Log.h"


namespace Dive
{
	Camera::Camera(Context * context, GameObject * owner)
		: Component(context, owner)
	{
		m_componentType = eComponentType::Camera;
		m_projectionType = eProjectionType::Perspective;

		DirectX::XMStoreFloat4x4(&m_view, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixIdentity());

		m_fovHorizontal = DirectX::XMConvertToRadians(90.0f);
		m_nearPlane = 0.3f;
		m_farPlane = 1000.0f;

		m_backgroundColor = DirectX::XMFLOAT4(0.396f, 0.611f, 0.937f, 1.0f);

		// 임시
		m_transform = m_owner->GetTransform();

		// 변환
	}

	Camera::~Camera()
	{
	}

	void Camera::OnUpdate()
	{
		// projection type에 따라 달라지는 부분이 있을 것이다.
	}

	void Camera::computeViewMatrix()
	{
		// 언제 획득케 할 것인가?
		if (!m_transform)
		{
			CORE_ERROR("");
			return;
		}

		auto pos	= m_transform->GetPosition();
		auto up		= m_transform->GetUp();
		auto lookAt = m_transform->GetForward();

		DirectX::XMStoreFloat4x4(&m_view, DirectX::XMMatrixLookAtLH(pos, lookAt, up));
	}

	void Camera::computeProjectionMatrix()
	{
		if (m_projectionType == eProjectionType::Orthographic)
		{

		}
		else if (m_projectionType == eProjectionType::Perspective)
		{

		}
		else
		{
			CORE_ERROR("");
			return;
		}
	}
}