#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

namespace dive
{
	Camera::Camera(GameObject* pGameObject)
		: Component(typeid(Camera).hash_code(), pGameObject)
	{
		m_ProjectionType = eProjectionType::Perspective;

		m_FieldOfView = 0.0f;
		m_ViewWidth = 0.0f;
		m_ViewHeight = 0.0f;
		m_NearPlane = 0.1f;
		m_FarPlane = 1000.0f;

		m_bUpdated = false;

		m_BackgroundColor = DirectX::XMFLOAT4(0.396f, 0.611f, 0.937f, 1.0f);
		DirectX::XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	}
	
	Camera::~Camera()
	{
	}

	void Camera::Update(float deltaTime)
	{
		// viewport�� position, rotation���ε� updated�� Ȯ���Ѵ�...

		if (!m_bUpdated)
			return;

		DirectX::XMStoreFloat4x4(&m_View, computeViewMatrix());
		DirectX::XMStoreFloat4x4(&m_Projection, computeProjectionMatrix());
		// view * projection
		// frustum??

		m_bUpdated = false;
	}

	void Camera::SetProjectionType(eProjectionType type)
	{
		if (m_ProjectionType == type)
			return;

		m_ProjectionType = type;
		m_bUpdated = true;
	}

	void Camera::SetFieldOfView(float angle)
	{
		if (m_FieldOfView == angle)
			return;

		m_FieldOfView = angle;
		m_bUpdated = true;
	}

	void Camera::SetViewWidth(float width)
	{
		if (m_ViewWidth == width)
			return;

		m_ViewWidth = width;
		m_bUpdated = true;
	}

	void Camera::SetViewHeight(float height)
	{
		if (m_ViewHeight == height)
			return;

		m_ViewHeight = height;
		m_bUpdated = true;
	}

	void Camera::SetNearPlane(float nearPlane)
	{
		if (m_NearPlane == nearPlane)
			return;

		// �Ѱ�ġ�� ���� �� ����. ��� ������ �ȵȴ�.

		m_NearPlane = nearPlane;
		m_bUpdated = true;
	}

	void Camera::SetFarPlane(float farPlane)
	{
		if (m_FarPlane == farPlane)
			return;

		// �̰͵� ��� NearPlane���� Ŀ���� �Ŵ�.

		m_FarPlane = farPlane;
		m_bUpdated = true;
	}

	void Camera::SetBackgroundColor(const DirectX::XMFLOAT4& color)
	{
		if (DirectX::XMVector4Equal(DirectX::XMLoadFloat4(&m_BackgroundColor), DirectX::XMLoadFloat4(&color)))
			return;

		m_BackgroundColor = color;
	}

	DirectX::XMMATRIX Camera::computeViewMatrix() const
	{
		auto transform = GetGameObject()->GetTransform();
		
		auto pos = transform->GetPositionVector();
		// �ٶ󺸴� ���� = ȸ�� ����� * ���� ����
		DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		lookAt += pos;
		// ���� ���� = ȸ�� ����� * ���� ����
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		
		// �� ������ ���� �̿��� LookAtLH ���
		return DirectX::XMMatrixLookAtLH(pos, lookAt, up);
	}

	DirectX::XMMATRIX Camera::computeProjectionMatrix() const
	{
		if (m_ProjectionType == eProjectionType::Perspective)
		{
			return DirectX::XMMatrixPerspectiveFovLH(m_FieldOfView, GetAspect(), m_NearPlane, m_FarPlane);
		}
		else if (m_ProjectionType == eProjectionType::Orthographic)
		{
			return DirectX::XMMatrixOrthographicLH(m_ViewWidth, m_ViewHeight, m_NearPlane, m_FarPlane);
		}

		CORE_ERROR("");
		return DirectX::XMMatrixIdentity();
	}
}