#include "DivePch.h"
#include "Camera.h"
#include "Transform.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/RenderTexture.h"
#include "Scene/GameObject.h"
#include "Core/Log.h"
#include "Core/Timer.h"

namespace Dive
{
	static constexpr float FOV_MIN = 1.0f;
	static constexpr float FOV_MAX = 160.0f;

	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject)
		, m_pRenderTarget(nullptr)
	{
		m_ProjectionType = eProjectionType::Perspective;
		m_BackgroundColor = { 1.0f, 1.0, 1.0f, 1.0f };
		m_FieldOfView = 45.0f;
		m_NearClipPlane = 0.1f;
		m_FarClipPlane = 5000.0f;
		m_MoveSpeed = 10.0f;
		m_RotateSpeed = 50.0f;
		m_Viewport = {0, 0, (LONG)Graphics::GetResolutionWidth(), (LONG)Graphics::GetResolutionHeight()};
	}

	Camera::~Camera()
	{
		DV_DELETE(m_pRenderTarget);
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
		auto viewSize = GetRenderTargetSize();

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

	void Camera::SetBackgroundColor(float r, float g, float b, float a)
	{
		m_BackgroundColor = { r, g, b, a };
	}

	float Camera::GetAspectRatio() const
	{
		auto viewSize = GetRenderTargetSize();
		return viewSize.x / viewSize.y;
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

	ID3D11RenderTargetView* Camera::GetRenderTargetView() const
	{
		return m_pRenderTarget ? m_pRenderTarget->GetRenderTargetView() : Graphics::GetDefaultRenderTargetView();
	}

	DirectX::XMFLOAT2 Camera::GetRenderTargetSize() const
	{
		return m_pRenderTarget ? m_pRenderTarget->GetSize() : Graphics::GetResolution();
	}
}