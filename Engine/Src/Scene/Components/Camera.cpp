#include "DivePch.h"
#include "Camera.h"
#include "Transform.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Scene/GameObject.h"
#include "Core/Log.h"

namespace Dive
{
	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject)
		, m_bOrthographic(false)
		, m_ViewWidth(800.0f)
		, m_ViewHeight(600.0f)
		, m_BackgroundColor(1.0f, 1.0f, 1.0f, 1.0f)
		, m_FieldOfView(45.0f)
		, m_NearClipPlane(0.1f)
		, m_FarClipPlane(5000.0f)
		, m_ViewportRectRateX(0.0f)
		, m_ViewportRectRateY(0.0f)
		, m_ViewportRectRateHeight(1.0f)
		, m_ViewportRectRateWidth(1.0f)
		, m_RenderingPath(eRenderingPath::Deferred)
	{
	}

	Camera::~Camera()
	{
	}

	DirectX::XMMATRIX Camera::GetViewMatrix() const
	{
		DV_CORE_ASSERT(m_pGameObject);

		auto pTransform = m_pGameObject->GetComponent<Transform>();
		if (!pTransform)
			return DirectX::XMMatrixIdentity();

		auto lookAt = pTransform->GetLookAt();

		// ���ó� �� �������� lookAt�� Camera�� �����ϴ�.
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

	// ���� ����ε� �׽�Ʈ�� ���ߴ�.
	DirectX::XMMATRIX Camera::GetOrthographicProjMatrix() const
	{
		return DirectX::XMMatrixOrthographicLH(
			//m_ViewWidth,
			//m_ViewHeight,
			m_ViewportRectRateHeight,
			m_ViewportRectRateWidth,
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

		return ((width * m_ViewportRectRateWidth) - (width * m_ViewportRectRateX)) /
			((height * m_ViewportRectRateHeight) - (height * m_ViewportRectRateY));
	}

	// min, max�� ���� ������ ���� ���� ����.
	void Camera::SetFieldOfView(float fov)
	{
		if (fov < 0.0f)
			m_FieldOfView = 1.0f;
		else if (fov > 160.0f)
			m_FieldOfView = 160.0f;
		else
			m_FieldOfView = fov;
	}

	void Camera::GetViewportRectRate(float& outX, float& outY, float& outWidth, float& outHeight)
	{
		outX = m_ViewportRectRateX;
		outY = m_ViewportRectRateY;
		outWidth = m_ViewportRectRateWidth;
		outHeight = m_ViewportRectRateHeight;
	}
	
	void Camera::SetViewportRectRate(float x, float y, float width, float height)
	{
		if ((x < 0.0f || x >= 1.0f) || (y < 0.0f || y >= 1.0f)
			|| (width < 0.0f || width > 1.0f || 0.0f >= width - x) || (height < 0.0f || height > 1.0f))
			return;

		m_ViewportRectRateX = x;
		m_ViewportRectRateY = y;
		m_ViewportRectRateWidth = width;
		m_ViewportRectRateHeight = height;
	}

	D3D11_VIEWPORT Camera::GetViewport()
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = static_cast<float>(Graphics::GetWindowWidth()) * m_ViewportRectRateX;
		viewport.TopLeftY = static_cast<float>(Graphics::GetWindowHeight()) * m_ViewportRectRateY;
		viewport.Width = static_cast<float>(Graphics::GetWindowWidth()) * m_ViewportRectRateWidth;
		viewport.Height = static_cast<float>(Graphics::GetWindowHeight()) * m_ViewportRectRateHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		return viewport;
	}
}