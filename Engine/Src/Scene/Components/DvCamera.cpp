#include "DivePch.h"
#include "DvCamera.h"
#include "DvTransform.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/DvRenderTexture.h"
#include "Renderer/Renderer.h"
#include "Renderer/Skydome.h"
#include "Scene/DvGameObject.h"
#include "Core/Log.h"
#include "Core/Timer.h"
#include "Input/Input.h"

namespace Dive
{
	static constexpr float FOV_MIN = 1.0f;
	static constexpr float FOV_MAX = 160.0f;

	DvCamera::DvCamera(DvGameObject* pGameObject)
		: DvComponent(pGameObject)
		, m_pRenderTarget(nullptr)
	{
		m_ProjectionType = eProjectionType::Perspective;
		m_BackgroundColor[0] = m_BackgroundColor[1] = m_BackgroundColor[2] = m_BackgroundColor[3] = 1.0f;
		m_FieldOfView = 45.0f;
		m_NearClipPlane = 0.1f;
		m_FarClipPlane = 5000.0f;
		m_MoveSpeed = 10.0f;
		m_RotateSpeed = 50.0f;
		m_Viewport = {0, 0, (LONG)GraphicsDevice::GetBackbufferWidth(), (LONG)GraphicsDevice::GetBackbufferHeight()};
	}

	DvCamera::~DvCamera()
	{
		DV_DELETE(m_pRenderTarget);
	}

	DirectX::XMFLOAT3 DvCamera::GetPosition()
	{
		auto pTransform = GetTransform();
		if (!pTransform)
			return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		return pTransform->GetPosition();
	}

	DirectX::XMMATRIX DvCamera::GetWorldMatrix()
	{
		auto pTransform = GetTransform();
		if (!pTransform)
			return DirectX::XMMatrixIdentity();

		return DirectX::XMMatrixTranslationFromVector(pTransform->GetPositionVector());
	}

	DirectX::XMMATRIX DvCamera::GetViewMatrix()
	{
		auto pTransform = GetTransform();
		if (!pTransform)
			return DirectX::XMMatrixIdentity();

		return DirectX::XMMatrixLookToLH(
			pTransform->GetPositionVector(),
			pTransform->GetForwardVector(),
			pTransform->GetUpwardVector());
	}

	DirectX::XMMATRIX DvCamera::GetProjectionMatrix() const
	{
		return m_ProjectionType == eProjectionType::Orthographic ? GetOrthographicProjMatrix() : GetPerspectiveProjMatrix();
	}

	// 아직 제대로된 테스트도 못했다.
	DirectX::XMMATRIX DvCamera::GetOrthographicProjMatrix() const
	{
		auto viewSize = GetRenderTargetSize();

		return DirectX::XMMatrixOrthographicLH(
			viewSize.x,
			viewSize.y,
			m_NearClipPlane,
			m_FarClipPlane);
	}

	DirectX::XMMATRIX DvCamera::GetPerspectiveProjMatrix() const
	{
		return DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(m_FieldOfView),
			GetAspectRatio(),
			m_NearClipPlane,
			m_FarClipPlane);
	}

	void DvCamera::SetBackgroundColor(float r, float g, float b, float a)
	{
		m_BackgroundColor[0] = r;	m_BackgroundColor[1] = g;	m_BackgroundColor[2] = b;	m_BackgroundColor[3] = a;
	}

	float DvCamera::GetAspectRatio() const
	{
		auto viewSize = GetRenderTargetSize();
		return viewSize.x / viewSize.y;
	}

	void DvCamera::SetFieldOfView(float fov)
	{
		if (fov < FOV_MIN)
			m_FieldOfView = 1.0f;
		else if (fov > FOV_MAX)
			m_FieldOfView = 160.0f;
		else
			m_FieldOfView = fov;
	}

	ID3D11RenderTargetView* DvCamera::GetRenderTargetView() const
	{
		return m_pRenderTarget ? m_pRenderTarget->GetRenderTargetView() : GraphicsDevice::GetDefaultRenderTargetView();
	}

	DirectX::XMFLOAT2 DvCamera::GetRenderTargetSize() const
	{
		return m_pRenderTarget ? m_pRenderTarget->GetSize() : GraphicsDevice::GetBackbufferSize();
	}
}