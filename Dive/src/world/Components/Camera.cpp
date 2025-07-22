#include "stdafx.h"
#include "Camera.h"
#include "Transform.h"
#include "../GameObject.h"
#include "graphics/RenderTexture.h"
#include "rendering/Renderer.h"

using namespace DirectX;

namespace Dive
{
	static constexpr float FOV_MIN = 1.0f;
	static constexpr float FOV_MAX = 160.0f;

	static constexpr float NEAR_CLIP_PLANE = 0.1f;

	std::vector<Camera*> Camera::s_AllCameras;

	Camera::Camera(GameObject* gameObject)
		: Component(gameObject)
	{
		s_AllCameras.push_back(this);
		DV_LOG(Camera, info, "생성: {}, {}", GetName(), GetInstanceID());
	}

	Camera::~Camera()
	{
		auto it = std::find(s_AllCameras.begin(), s_AllCameras.end(), this);
		if (it != s_AllCameras.end())
			s_AllCameras.erase(it);
		
		DV_LOG(Camera, info, "소멸: {}, {}", GetName(), GetInstanceID());
	}

	void Camera::Update()
	{
		m_Frustum.Update(GetView(), GetProjection());
	}

	void Camera::Bind(ID3D11DeviceContext* deviceContext) const
	{
		assert(deviceContext);

		auto renderTargetView = GetRenderTarget()->GetRenderTargetView();
		auto depthStencilView = GetRenderTarget()->GetDepthStencilView();
		auto color = GetBackgroundColor();

		deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		FLOAT clearColor[4]{ color.x, color.y, color.z, color.w };
		deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
		deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		auto viewport = GetViewport();
		deviceContext->RSSetViewports(1, &viewport);		
	}

	DirectX::XMFLOAT4X4 Camera::GetView() const
	{
		auto transform = GetGameObject()->GetTransform();

		XMFLOAT3 position = transform->GetPosition();
		XMVECTOR positionVec = XMLoadFloat3(&position);

		XMFLOAT3 forward = transform->GetForward();
		XMVECTOR forwardVec = XMLoadFloat3(&forward);
		
		XMFLOAT3 up = transform->GetUp();
		XMVECTOR upVec = XMLoadFloat3(&up);

		XMMATRIX viewMatrix = XMMatrixLookToLH(positionVec, forwardVec, upVec);
		XMFLOAT4X4 view{};
		XMStoreFloat4x4(&view, viewMatrix);	

		return view;
	}

	DirectX::XMMATRIX Camera::GetViewMatrix() const
	{
		auto view = GetView();
		return XMLoadFloat4x4(&view);
	}

	DirectX::XMFLOAT4X4 Camera::GetProjection() const
	{
		XMFLOAT4X4 projection{};

		if (m_ProjectionType == eProjectionType::Perspective)
		{
			auto projectionMatrix =  XMMatrixPerspectiveFovLH(
				3.141592654f / 4.0f,//XMConvertToRadians(m_FieldOfView),
				GetAspectRatio(),
				m_NearClipPlane,
				m_FarClipPlane);

			XMStoreFloat4x4(&projection, projectionMatrix);
		}
		else if (m_ProjectionType == eProjectionType::Orthographic)
		{
			// 코파일럿이 작성한  코드다.
			// width, height가 맞는지 모르겠다.
			auto projectionMatrix = XMMatrixOrthographicLH(
				GetAspectRatio() * m_FarClipPlane,
				m_FarClipPlane,
				m_NearClipPlane,
				m_FarClipPlane);

			XMStoreFloat4x4(&projection, projectionMatrix);
		}

		return projection;
	}

	DirectX::XMMATRIX Camera::GetProjectionMatrix() const
	{
		auto projection = GetProjection();
		return XMLoadFloat4x4(&projection);
	}

	float Camera::GetAspectRatio() const
	{
		auto viewport = GetViewport();
		return (viewport.Width / viewport.Height);
	}
	
	void Camera::SetFieldOfView(float fov)
	{
		if (fov < FOV_MIN)
			m_FieldOfView = FOV_MIN;
		else if (fov > FOV_MAX)
			m_FieldOfView = FOV_MAX;
		else
			m_FieldOfView = fov;
	}

	void Camera::SetNearClipPlane(float nearPlane)
	{
		if (nearPlane < NEAR_CLIP_PLANE)
		{
			DV_LOG(Camera, warn, "유효하지 않은 Near Clip 값 전달: {}", nearPlane);
			return;
		}
		m_NearClipPlane = nearPlane;
	}

	void Camera::SetFarClipPlane(float farPlane)
	{
		if (farPlane <= m_NearClipPlane)
		{
			DV_LOG(Camera, warn, "유효하지 않은 Far Clip 값 전달: {}", farPlane);
			return;
		}
		m_FarClipPlane = farPlane;
	}

	void Camera::GetViewportRect(float& outLeft, float& outTop, float& outRight, float& outBottom) const
	{
		outLeft = m_ViewportLeft;
		outTop = m_ViewportTop;
		outRight = m_ViewportRight;
		outBottom = m_ViewportBottom;
	}

	void Camera::SetViewportRect(float left, float top, float right, float bottom)
	{
		if (left < 0.0f || left > right ||
			top < 0.0f || top > bottom ||
			right <= left || right > 1.0f ||
			bottom <= top || bottom > 1.0f)
		{
			DV_LOG(Camera, warn, "유효하지 않은 뷰포트 설정값 전달: {}, {}, {}, {}", left, top, right, bottom);
			return;
		}

		m_ViewportLeft = left;
		m_ViewportTop = top;
		m_ViewportRight = right;
		m_ViewportBottom = bottom;
	}

	void Camera::SetViewportTop(float top)
	{
		if (top < 0.0f || top > m_ViewportBottom)
		{
			DV_LOG(Camera, warn, "유효하지 않은 뷰포트 Top 값 전달: {}", top);
			return;
		}
		m_ViewportTop = top;
	}

	void Camera::SetViewportLeft(float left)
	{
		if (left < 0.0f || left > m_ViewportRight)
		{
			DV_LOG(Camera, warn, "유효하지 않은 뷰포트 Left 값 전달: {}", left);
			return;
		}
		m_ViewportLeft = left;
	}

	void Camera::SetViewportRight(float right)
	{
		if (right < m_ViewportLeft || right > 1.0f)
		{
			DV_LOG(Camera, warn, "유효하지 않은 뷰포트 Right 값 전달: {}", right);
			return;
		}
		m_ViewportRight = right;
	}

	void Camera::SetViewportBottom(float bottom)
	{
		if (bottom < m_ViewportTop || bottom > 1.0f)
		{
			DV_LOG(Camera, warn, "유효하지 않은 뷰포트 Bottom 값 전달: {}", bottom);
			return;
		}
		m_ViewportBottom = bottom;
	}

	D3D11_VIEWPORT Camera::GetViewport() const
	{
		auto renderTargetWidth = static_cast<float>(GetRenderTarget()->GetWidth());
		auto renderTargetHeight = static_cast<float>(GetRenderTarget()->GetHeight());

		float topLeftX = m_ViewportLeft * renderTargetWidth;
		float topLeftY = m_ViewportTop * renderTargetHeight;
		float width = (m_ViewportRight - m_ViewportLeft) * renderTargetWidth;
		float height = (m_ViewportBottom - m_ViewportTop) * renderTargetHeight;

		return D3D11_VIEWPORT{
			topLeftX,
			topLeftY,
			width,
			height,
			0.0f,
			1.0f
		};
	}

	RenderTexture* Camera::GetRenderTarget() const
	{
		return m_RenderTarget ? m_RenderTarget : Renderer::GetRenderTarget(eRenderTarget::FrameOutput);
	}

	Camera* Camera::GetMainCamera()
	{
		for (auto camera : s_AllCameras)
		{
			if (camera->GetGameObject()->GetTag() == "MainCamera")
				return camera;
		}

		return nullptr;
	}
}
