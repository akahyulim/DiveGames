#include "divepch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	Camera::Camera(Context* pContext)
		: Component(pContext),
		m_bOrthographic(false),
		m_OrthoViewSize(1.0f, 1.0f),
		m_FieldOfView(45.0f),
		m_AspectRatio(1.0f),
		m_NearClipPlane(0.1f),
		m_FarClipPlane(1000.0f)
	{
	}

	Camera::~Camera()
	{
	}

	DirectX::XMMATRIX Camera::GetViewMatrix() const
	{
		auto pTransform = m_pGameObject->GetComponent<Transform>();
		if (!pTransform)
			return DirectX::XMMatrixIdentity();

		auto position = pTransform->GetPosition();
		auto focus = pTransform->GetForward();
		auto up = pTransform->GetUp();

		return DirectX::XMMatrixLookAtLH(
			DirectX::XMLoadFloat3(&position),
			DirectX::XMLoadFloat3(&focus),
			DirectX::XMLoadFloat3(&up));
	}
	
	DirectX::XMMATRIX Camera::GetProjectionMatrix() const
	{
		return m_bOrthographic ? GetOrthographicProjMatrix() : GetPerspectiveProjMatrix();
	}

	DirectX::XMMATRIX Camera::GetOrthographicProjMatrix() const
	{
		return DirectX::XMMatrixOrthographicLH(
			m_OrthoViewSize.x,
			m_OrthoViewSize.y,
			m_NearClipPlane,
			m_FarClipPlane);
	}

	DirectX::XMMATRIX Camera::GetPerspectiveProjMatrix() const
	{
		return DirectX::XMMatrixPerspectiveFovLH(
			m_FieldOfView,
			m_AspectRatio,
			m_NearClipPlane,
			m_FarClipPlane);
	}

	void Camera::SetOrthoViewSize(float size)
	{
		m_OrthoViewSize.x = size;
		m_OrthoViewSize.y = size;
		m_AspectRatio = 1.0f;
	}

	void Camera::SetOrthoViewSize(const DirectX::XMFLOAT2& size)
	{
		m_OrthoViewSize.x = size.x;
		m_OrthoViewSize.y = size.y;
		m_AspectRatio = size.x / size.y;
	}

	// min, max를 따로 선언해 놓는 편이 낫다.
	void Camera::SetFieldOfView(float fov)
	{
		if (fov < 0.0f)
			m_FieldOfView = 0.0f;
		else if (fov > 160.0f)
			m_FieldOfView = 160.0f;
		else
			m_FieldOfView = fov;
	}
}