#include "DivePch.h"
#include "Camera.h"
#include "Transform.h"
#include "Core/CoreDefs.h"
#include "Scene/GameObject.h"
#include "IO/Log.h"

namespace Dive
{
	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject),
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
		DV_CORE_TRACE("Destroy Camera: {:s}", GetName());
	}

	DirectX::XMMATRIX Camera::GetViewMatrix() const
	{
		DV_ASSERT(m_pGameObject);

		auto pTransform = m_pGameObject->GetComponent<Transform>();
		if (!pTransform)
			return DirectX::XMMatrixIdentity();

		auto lookAt = pTransform->GetLookAt();

		// 역시나 이 곳에서도 lookAt은 Camera와 동일하다.
		DirectX::XMFLOAT3 lk;
		DirectX::XMStoreFloat3(&lk, lookAt);
		//DV_LOG_ENGINE_DEBUG("Camera's LookAt: {0:f}, {1:f}, {2:f}", lk.x, lk.y, lk.z);

		return DirectX::XMMatrixLookAtLH(
			pTransform->GetPosition(),
			//DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			//pTransform->GetLookAt(),
			lookAt,
			//DirectX::XMVectorAdd(pTransform->GetForward(), pTransform->GetPosition()),
			pTransform->GetUp());
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