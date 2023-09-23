#include "DivePch.h"
#include "Camera.h"
#include "Transform.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Scene/GameObject.h"
#include "IO/Log.h"

namespace Dive
{
	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject),
		m_bOrthographic(false),
		m_ViewWidth(800.0f),
		m_ViewHeight(600.0f),
		m_FieldOfView(45.0f),
		m_NearClipPlane(0.1f),
		m_FarClipPlane(5000.0f),
		m_ViewportRectX(0.0f),
		m_ViewportRectY(0.0f),
		m_ViewportRectH(1.0f),
		m_ViewportRectW(1.0f)
	{
	}

	Camera::~Camera()
	{
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
			m_ViewWidth,
			m_ViewHeight,
			m_NearClipPlane,
			m_FarClipPlane);
	}

	DirectX::XMMATRIX Camera::GetPerspectiveProjMatrix() const
	{
		return DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(m_FieldOfView),
			GetAspectRatio(),
			m_NearClipPlane,
			m_FarClipPlane);
	}

	float Camera::GetAspectRatio() const
	{
		float width = static_cast<float>(Graphics::GetWindowWidth());
		float height = static_cast<float>(Graphics::GetWindowHeight());

		return ((width * m_ViewportRectW) - (width * m_ViewportRectX)) /
			((height * m_ViewportRectH) - (height * m_ViewportRectY));
	}

	// min, max를 따로 선언해 놓는 편이 낫다.
	void Camera::SetFieldOfView(float fov)
	{
		if (fov < 0.0f)
			m_FieldOfView = 1.0f;
		else if (fov > 160.0f)
			m_FieldOfView = 160.0f;
		else
			m_FieldOfView = fov;
	}

	void Camera::GetViewportRect(float& x, float& y, float& w, float& h)
	{
		x = m_ViewportRectX;
		y = m_ViewportRectY;
		w = m_ViewportRectW;
		h = m_ViewportRectH;
	}
	
	void Camera::SetViewportRect(float x, float y, float w, float h)
	{
		if ((x < 0.0f || x >= m_ViewportRectW) || (y < 0.0f || y >= m_ViewportRectH) || 
			(w <= m_ViewportRectX || w > 1.0f) || (h <= m_ViewportRectY || h > 1.0f))
			return;

		m_ViewportRectX = x;
		m_ViewportRectY = y;
		m_ViewportRectW = w;
		m_ViewportRectH = h;
	}

	D3D11_VIEWPORT Camera::GetViewport()
	{
		D3D11_VIEWPORT viewport;

		viewport.TopLeftX = static_cast<float>(Graphics::GetWindowWidth()) * m_ViewportRectX;
		viewport.TopLeftY = static_cast<float>(Graphics::GetWindowHeight()) * m_ViewportRectY;
		viewport.Width = static_cast<float>(Graphics::GetWindowWidth()) * m_ViewportRectW;
		viewport.Height = static_cast<float>(Graphics::GetWindowHeight()) * m_ViewportRectH;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		return viewport;
	}
}