#include "stdafx.h"
#include "Camera.h"
#include "Transform.h"
#include "../GameObject.h"
#include "graphics/RenderTexture.h"
#include "graphics/ConstantBuffer.h"
#include "graphics/Graphics.h"
#include "rendering/Renderer.h"

using namespace DirectX;

namespace Dive
{
	static constexpr float FOV_MIN = 1.0f;
	static constexpr float FOV_MAX = 160.0f;

	static constexpr float NEAR_CLIP_PLANE = 0.1f;

	std::vector<Camera*> Camera::s_allCameras;

	Camera::Camera(GameObject* gameObject)
		: Component(gameObject)
	{
		s_allCameras.push_back(this);

		m_cbCamera = std::make_unique<ConstantBuffer>(static_cast<uint32_t>(sizeof(CameraData)));
		if (!m_cbCamera) DV_LOG(MeshRenderer, err, "[::Camera] cbCamera 생성 실패");

		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(Graphics::GetWidth());
		m_viewport.Height = static_cast<float>(Graphics::GetHeight());
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		DV_LOG(Camera, info, "생성: {}, {}", GetName(), GetInstanceID());
	}

	Camera::~Camera()
	{
		auto it = std::find(s_allCameras.begin(), s_allCameras.end(), this);
		if (it != s_allCameras.end())
			s_allCameras.erase(it);
		
		DV_LOG(Camera, info, "소멸: {}, {}", GetName(), GetInstanceID());
	}

	void Camera::Update()
	{
		m_frustum.Update(GetView(), GetProjection());
	}

	void Camera::Bind()
	{
		CameraData data{};
		
		XMStoreFloat4x4(&data.viewMatrix, XMMatrixTranspose(GetViewMatrix()));
		XMStoreFloat4x4(&data.projMatrix, XMMatrixTranspose(GetProjectionMatrix()));
		XMStoreFloat4x4(&data.viewProjMatrix, XMMatrixTranspose(GetViewMatrix() * GetProjectionMatrix()));
		
		const auto& pos = GetTransform()->GetPosition();
		data.cameraPosition = XMFLOAT4(pos.x, pos.y, pos.z, 1.0f);
		
		data.backgroundColor = GetBackgroundColorByXMFLOAT4();

		m_cbCamera->Update<CameraData>(data);

		m_cbCamera->BindVS(eCBufferSlotVS::Camera);
		m_cbCamera->BindPS(eCBufferSlotPS::Camera);

		auto rtv = m_renderTarget ? m_renderTarget->GetRenderTargetView() : Graphics::GetRenderTargetView();
		auto dsv = m_renderTarget? m_renderTarget->GetDepthStencilView() : Graphics::GetDepthStencilView();

		Graphics::GetDeviceContext()->OMSetRenderTargets(1, &rtv, dsv);
		Graphics::GetDeviceContext()->ClearRenderTargetView(rtv, m_backgroundColor);
		Graphics::GetDeviceContext()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

		// RenderTarget이 Resize되면 Viewport도 달라져야 한다.
		SetViewport(0.0f, 0.0f, static_cast<float>(m_renderTarget->GetWidth()), static_cast<float>(m_renderTarget->GetHeight()));
		Graphics::GetDeviceContext()->RSSetViewports(1, &m_viewport);
	}

	DirectX::XMFLOAT4X4 Camera::GetView() const
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, GetViewMatrix());
		return result;
	}

	DirectX::XMMATRIX Camera::GetViewMatrix() const
	{
		auto transform = GetGameObject()->GetTransform();
		assert(transform);

		return XMMatrixLookToLH(transform->GetPositionVector(), transform->GetLocalForward(), transform->GetLocalUp());
	}

	DirectX::XMFLOAT4X4 Camera::GetProjection() const
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, GetProjectionMatrix());
		return result;
	}

	DirectX::XMMATRIX Camera::GetProjectionMatrix() const
	{
		if (m_projectionType == eProjectionType::Perspective)
		{
			return XMMatrixPerspectiveFovLH(
				XMConvertToRadians(m_fieldOfView),
				GetAspectRatio(),
				m_nearClipPlane,
				m_farClipPlane);
		}
		else
		{		
			// 코파일럿이 작성한  코드다.
			// width, height가 맞는지 모르겠다.
			return XMMatrixOrthographicLH(
				GetAspectRatio() * m_farClipPlane,
				m_farClipPlane,
				m_nearClipPlane,
				m_farClipPlane);
		}
	}

	float Camera::GetAspectRatio() const
	{
		return (m_viewport.Width / m_viewport.Height);
	}
	
	void Camera::SetFieldOfView(float fov)
	{
		if (fov < FOV_MIN)
			m_fieldOfView = FOV_MIN;
		else if (fov > FOV_MAX)
			m_fieldOfView = FOV_MAX;
		else
			m_fieldOfView = fov;
	}

	void Camera::SetNearClipPlane(float nearPlane)
	{
		if (nearPlane < NEAR_CLIP_PLANE)
		{
			DV_LOG(Camera, warn, "[::SetNearClipPlane] 유효하지 않은 Near Clip 값: {}", nearPlane);
			return;
		}
		m_nearClipPlane = nearPlane;
	}

	void Camera::SetFarClipPlane(float farPlane)
	{
		if (farPlane <= m_nearClipPlane)
		{
			DV_LOG(Camera, warn, "유효하지 않은 Far Clip 값 전달: {}", farPlane);
			return;
		}
		m_farClipPlane = farPlane;
	}

	void Camera::GetViewportRect(float& outLeft, float& outTop, float& outRight, float& outBottom) const
	{
		outLeft = m_viewportLeft;
		outTop = m_viewportTop;
		outRight = m_viewportRight;
		outBottom = m_viewportBottom;
	}

	void Camera::SetViewportRect(float left, float top, float right, float bottom)
	{
		if (left < 0.0f || left > right ||
			top < 0.0f || top > bottom ||
			right <= left || right > 1.0f ||
			bottom <= top || bottom > 1.0f)
		{
			DV_LOG(Camera, warn, "[::SetViewportRect] 유효하지 않은 뷰포트 설정값: {}, {}, {}, {}", left, top, right, bottom);
			return;
		}

		m_viewportLeft = left;
		m_viewportTop = top;
		m_viewportRight = right;
		m_viewportBottom = bottom;
	}

	void Camera::SetViewportTop(float top)
	{
		if (top < 0.0f || top > m_viewportBottom)
		{
			DV_LOG(Camera, warn, "[::SetViewportTop] 유효하지 않은 값: {}", top);
			return;
		}
		m_viewportTop = top;
	}

	void Camera::SetViewportLeft(float left)
	{
		if (left < 0.0f || left > m_viewportRight)
		{
			DV_LOG(Camera, warn, "[::SetViewportLeft] 유효하지 않은 값: {}", left);
			return;
		}
		m_viewportLeft = left;
	}

	void Camera::SetViewportRight(float right)
	{
		if (right < m_viewportLeft || right > 1.0f)
		{
			DV_LOG(Camera, warn, "[::SetViewportRight] 유효하지 않은 값: {}", right);
			return;
		}
		m_viewportRight = right;
	}

	void Camera::SetViewportBottom(float bottom)
	{
		if (bottom < m_viewportTop || bottom > 1.0f)
		{
			DV_LOG(Camera, warn, "[::SetViewportBottom] 유효하지 않은 값: {}", bottom);
			return;
		}
		m_viewportBottom = bottom;
	}

	void Camera::SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
	{
		m_viewport.TopLeftX = topLeftX;
		m_viewport.TopLeftY = topLeftY;
		m_viewport.Width = width;
		m_viewport.Height = height;
		m_viewport.MinDepth = minDepth;
		m_viewport.MaxDepth = maxDepth;
	}

	D3D11_VIEWPORT Camera::GetViewport() const
	{
		return m_viewport;
	}

	DirectX::XMFLOAT4 Camera::GetBackgroundColorByXMFLOAT4() const
	{
		return { m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], m_backgroundColor[3] };
	}

	void Camera::SetBackgroundColor(float r, float g, float b, float a)
	{
		m_backgroundColor[0] = r;
		m_backgroundColor[1] = g;
		m_backgroundColor[2] = b;
		m_backgroundColor[3] = a;
	}

	void Camera::SetBackgroundColor(const float* color)
	{
		m_backgroundColor[0] = color[0];
		m_backgroundColor[1] = color[1];
		m_backgroundColor[2] = color[2];
		m_backgroundColor[3] = color[3];
	}

	void Camera::SetBackgroundColor(const DirectX::XMFLOAT4& color)
	{
		m_backgroundColor[0] = color.x;
		m_backgroundColor[1] = color.y;
		m_backgroundColor[2] = color.z;
		m_backgroundColor[3] = color.w;
	}

	RenderTexture* Camera::GetRenderTarget() const
	{
		return m_renderTarget ? m_renderTarget : nullptr;// Renderer::GetRenderTarget(eRenderTarget::FrameOutput);
	}


	void Camera::SetRenderTarget(RenderTexture* renderTarget)
	{
		m_renderTarget = renderTarget;

		SetViewport(0.0f, 0.0f, static_cast<float>(m_renderTarget->GetWidth()), static_cast<float>(m_renderTarget->GetHeight()));
	}

	Camera* Camera::GetMainCamera()
	{
		for (auto camera : s_allCameras)
		{
			if (camera->GetGameObject()->GetTag() == "MainCamera")
				return camera;
		}

		return nullptr;
	}
}
