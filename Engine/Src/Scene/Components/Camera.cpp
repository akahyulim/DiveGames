#include "DivePch.h"
#include "Camera.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/RenderTexture.h"
#include "Scene/GameObject.h"

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
		m_FarClipPlane = 1000.0f;
		m_MoveSpeed = 10.0f;
		m_RotateSpeed = 50.0f;
		
		ZeroMemory(&m_CBufferVS, sizeof(VSConstBuf_Camera));
		ZeroMemory(&m_CBufferPS, sizeof(PSConstBuf_Camera));
	}

	Camera::~Camera()
	{
		DV_DELETE(m_pRenderTarget);

		DV_ENGINE_TRACE("컴포넌트({0:s}'s {1:s}) 소멸", GetName(), GetTypeName());
	}

	void Camera::Update()
	{
		//vs
		m_CBufferVS.view = DirectX::XMMatrixTranspose(GetViewMatrix());
		m_CBufferVS.projection = DirectX::XMMatrixTranspose(GetProjectionMatrix());

		// ps
		m_CBufferPS.position = GetPosition();
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMStoreFloat4x4(&proj, GetProjectionMatrix());
		m_CBufferPS.perspectiveValue.x = 1.0f / proj._11;
		m_CBufferPS.perspectiveValue.y = 1.0f / proj._22;
		m_CBufferPS.perspectiveValue.z = proj._43;
		m_CBufferPS.perspectiveValue.w = -proj._33;

		auto viewInv = DirectX::XMMatrixInverse(nullptr, GetViewMatrix());
		m_CBufferPS.viewInverse = DirectX::XMMatrixTranspose(viewInv);
	}

	DirectX::XMFLOAT3 Camera::GetPosition()
	{
		return m_pGameObject->GetPosition();
	}

	DirectX::XMMATRIX Camera::GetWorldMatrix()
	{
		return DirectX::XMMatrixTranslationFromVector(m_pGameObject->GetPositionVector());
	}

	DirectX::XMMATRIX Camera::GetViewMatrix()
	{
		const auto& pos = m_pGameObject->GetPositionVector();
		const auto& forward = m_pGameObject->GetForwardVector();
		const auto& up = m_pGameObject->GetUpwardVector();

		const auto& focus = DirectX::XMVectorAdd(pos, forward);

		return DirectX::XMMatrixLookAtLH(pos, focus, up);
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
		return m_pRenderTarget ? m_pRenderTarget->GetRenderTargetView() : Graphics::GetInstance()->GetDefaultRenderTargetView();
	}

	DirectX::XMFLOAT2 Camera::GetRenderTargetSize() const
	{
		return m_pRenderTarget ? m_pRenderTarget->GetSize() : Graphics::GetInstance()->GetResolution();
	}
}