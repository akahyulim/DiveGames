#include "DivePch.h"
#include "Camera.h"
#include "Transform.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Renderer/Renderer.h"
#include "Renderer/Skydome.h"
#include "Scene/GameObject.h"
#include "Core/Log.h"
#include "Core/Timer.h"
#include "Input/Input.h"

namespace Dive
{
	static constexpr float FOV_MIN = 1.0f;
	static constexpr float FOV_MAX = 160.0f;

	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject)
	{
		m_ProjectionType = eProjectionType::Perspective;
		m_BackgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_FieldOfView = 45.0f;
		m_NearClipPlane = 0.1f;
		m_FarClipPlane = 5000.0f;
		m_ViewportRectRateX = 0.0f;
		m_ViewportRectRateY = 0.0f;
		m_ViewportRectRateHeight = 1.0f;
		m_ViewportRectRateWidth = 1.0f;
		m_MoveSpeed = 10.0f;
		m_RotateSpeed = 50.0f;
		m_pSkydome = nullptr;
	}

	Camera::~Camera()
	{
		DV_DELETE(m_pSkydome);
	}

	DirectX::XMFLOAT3 Camera::GetPosition()
	{
		auto pTransform = GetTransform();
		if (!pTransform)
			return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		return pTransform->GetPosition();
	}

	DirectX::XMMATRIX Camera::GetWorldMatrix()
	{
		auto pTransform = GetTransform();
		if (!pTransform)
			return DirectX::XMMatrixIdentity();

		return DirectX::XMMatrixTranslationFromVector(pTransform->GetPositionVector());
	}

	DirectX::XMMATRIX Camera::GetViewMatrix()
	{
		auto pTransform = GetTransform();
		if (!pTransform)
			return DirectX::XMMatrixIdentity();

		return DirectX::XMMatrixLookToLH(
			pTransform->GetPositionVector(),
			pTransform->GetForwardVector(),
			pTransform->GetUpwardVector());
	}

	DirectX::XMMATRIX Camera::GetProjectionMatrix() const
	{
		return m_ProjectionType == eProjectionType::Orthographic ? GetOrthographicProjMatrix() : GetPerspectiveProjMatrix();
	}

	// 아직 제대로된 테스트도 못했다.
	DirectX::XMMATRIX Camera::GetOrthographicProjMatrix() const
	{
		auto viewSize = Renderer::GetResolutionRender();

		return DirectX::XMMatrixOrthographicLH(
			viewSize.x,
			viewSize.y,
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
		auto viewSize = Renderer::GetResolutionRender();

		return ((viewSize.x * m_ViewportRectRateWidth) - (viewSize.x * m_ViewportRectRateX)) /
			((viewSize.y * m_ViewportRectRateHeight) - (viewSize.y * m_ViewportRectRateY));
	}

	void Camera::SetFieldOfView(float fov)
	{
		if (fov < FOV_MIN)
			m_FieldOfView = 1.0f;
		else if (fov > FOV_MAX)
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

	D3D11_VIEWPORT Camera::GetViewport() const
	{
		auto viewSize = Renderer::GetResolutionRender();

		return D3D11_VIEWPORT
		{
			viewSize.x * m_ViewportRectRateX,
			viewSize.y * m_ViewportRectRateY,
			viewSize.x * m_ViewportRectRateWidth,
			viewSize.y * m_ViewportRectRateHeight,
			0.0f,
			1.0f,
		};
	}
}