#include "stdafx.h"
#include "Camera.h"
#include "Transform.h"
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

	Camera::Camera(GameObject* gameObject)
		: Component(gameObject)
	{
	}

	Camera::~Camera()
	{
		// 렌더타겟은 외부에서 가져오기 때문에 직접 제거하는 건 에바다.
	}

	// 버퍼를 생성 및 관리하는 것은 맞지 않다.
	// 갱신된 데이터로 버퍼를 업데이트하도록 구현해야 한다.
	void Camera::Update()
	{
	}

	DirectX::XMFLOAT3 Camera::GetPosition()
	{
		return m_GameObject->GetComponent<Transform>()->GetPosition();
	}

	DirectX::XMMATRIX Camera::GetSceneMatrix()
	{
		return DirectX::XMMatrixTranslationFromVector(m_GameObject->GetComponent<Transform>()->GetPositionVector());
	}

	DirectX::XMMATRIX Camera::GetViewMatrix()
	{
		const auto& pos = m_GameObject->GetComponent<Transform>()->GetPositionVector();
		const auto& forward = m_GameObject->GetComponent<Transform>()->GetForwardVector();
		const auto& focus = DirectX::XMVectorAdd(pos, forward);

		const auto& up = m_GameObject->GetComponent<Transform>()->GetUpwardVector();

		return DirectX::XMMatrixLookAtLH(pos, focus, up);
	}

	DirectX::XMMATRIX Camera::GetProjectionMatrix() const
	{
		return m_ProjectionType == eProjectionType::Orthographic ? GetOrthographicProjMatrix() : GetPerspectiveProjMatrix();
	}

	// 아직 제대로된 테스트도 못했다.
	DirectX::XMMATRIX Camera::GetOrthographicProjMatrix() const
	{
		auto viewSize = GetRenderTextureSize();

		return DirectX::XMMatrixOrthographicLH(
			static_cast<float>(viewSize.x),
			static_cast<float>(viewSize.y),
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
		auto viewSize = GetRenderTextureSize();
		return static_cast<float>(viewSize.x / viewSize.y);
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

	DirectX::XMUINT2 Camera::GetRenderTextureSize() const
	{
		if (m_RenderTexture)
			return { m_RenderTexture->GetWidth(), m_RenderTexture->GetHeight() };

		return {};
	}
}