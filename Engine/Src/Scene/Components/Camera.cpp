#include "DivePch.h"
#include "Camera.h"
#include "Core/CoreDefs.h"
#include "Graphics/Graphics.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/ConstantBuffer.h"
#include "Scene/GameObject.h"

namespace Dive
{
#pragma pack(push, 1)
	struct CB_CAMERA_VS
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};

	struct CB_CAMERA_DS
	{
		DirectX::XMMATRIX viewProj;
	};

	struct CB_CAMERA_PS
	{
		DirectX::XMFLOAT4 perspectiveValue;

		DirectX::XMMATRIX viewInverse;
	};
#pragma pack(pop)

	static constexpr float FOV_MIN = 1.0f;
	static constexpr float FOV_MAX = 160.0f;

	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject)
		, m_pRenderTarget(nullptr)
		, m_pCBufferVS(nullptr)
		, m_pCBufferDS(nullptr)
		, m_pCBufferPS(nullptr)
	{
		m_ProjectionType = eProjectionType::Perspective;
		m_BackgroundColor = { 1.0f, 1.0, 1.0f, 1.0f };
		m_FieldOfView = 45.0f;
		m_NearClipPlane = 0.1f;
		m_FarClipPlane = 1000.0f;
		m_MoveSpeed = 10.0f;
		m_RotateSpeed = 50.0f;
	}

	Camera::~Camera()
	{
		DV_DELETE(m_pCBufferPS);
		DV_DELETE(m_pCBufferDS);
		DV_DELETE(m_pCBufferVS);
		DV_DELETE(m_pRenderTarget);
	}

	// 버퍼를 생성 및 관리하는 것은 맞지 않다.
	// 갱신된 데이터로 버퍼를 업데이트하도록 구현해야 한다.
	void Camera::Update()
	{
		// vs constant buffer
		{
			if(!m_pCBufferVS)
				m_pCBufferVS = ConstantBuffer::Create("CB_CAMERA_VS", sizeof(CB_CAMERA_VS));

			auto pMappedData = static_cast<CB_CAMERA_VS*>(m_pCBufferVS->Map());
			pMappedData->view = DirectX::XMMatrixTranspose(GetViewMatrix());
			pMappedData->proj = DirectX::XMMatrixTranspose(GetProjectionMatrix());
			m_pCBufferVS->Unmap();
		}

		// ds constant buffer
		{
			if(!m_pCBufferDS)
				m_pCBufferDS = ConstantBuffer::Create("CB_CAMERA_DS", sizeof(CB_CAMERA_DS));

			auto pMappedData = static_cast<CB_CAMERA_DS*>(m_pCBufferDS->Map());
			pMappedData->viewProj = DirectX::XMMatrixTranspose(GetViewMatrix() * GetProjectionMatrix());
			m_pCBufferDS->Unmap();
		}

		// ps constant buffer
		{
			if(!m_pCBufferPS)
				m_pCBufferPS = ConstantBuffer::Create("CB_CAMERA_PS", sizeof(CB_CAMERA_PS));

			auto pMappedData = reinterpret_cast<CB_CAMERA_PS*>(m_pCBufferPS->Map());
			DirectX::XMFLOAT4X4 proj;
			DirectX::XMStoreFloat4x4(&proj, GetProjectionMatrix());
			pMappedData->perspectiveValue.x = 1.0f / proj._11;
			pMappedData->perspectiveValue.y = 1.0f / proj._22;
			pMappedData->perspectiveValue.z = proj._43;
			pMappedData->perspectiveValue.w = -proj._33;
			DirectX::XMMATRIX viewInv = DirectX::XMMatrixInverse(nullptr, GetViewMatrix());
			pMappedData->viewInverse = DirectX::XMMatrixTranspose(m_pGameObject->GetMatrix());
			m_pCBufferPS->Unmap();
		}
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
		const auto& focus = DirectX::XMVectorAdd(pos, forward);

		const auto& up = m_pGameObject->GetUpwardVector();

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