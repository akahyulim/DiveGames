#pragma once
#include "Component.h"
#include "math/Frustum.h"

namespace Dive
{
	class RenderTexture;
	class ConstantBuffer;

	enum class eProjectionType
	{
		Perspective,
		Orthographic
	};

	struct alignas(16) CameraData
	{
		DirectX::XMFLOAT3 position;
		float padding;
		DirectX::XMFLOAT3 perspectiveValue;
		DirectX::XMFLOAT4X4 viewInverse;
	};

	// https://docs.unity3d.com/ScriptReference/Camera.html
	class Camera : public Component
	{
	public:
		Camera(GameObject* gameObject);
		~Camera()override;

		void Update() override;

		void Bind() const;

		DirectX::XMFLOAT4X4 GetView() const;
		DirectX::XMMATRIX GetViewMatrix() const;
		DirectX::XMFLOAT4X4 GetProjection() const;
		DirectX::XMMATRIX GetProjectionMatrix() const;

		eProjectionType GetProjectionType() const { return m_projectionType; }
		void SetProjectionType(eProjectionType type) { m_projectionType = type; }
		
		float GetAspectRatio() const;

		float GetFieldOfView() const { return m_fieldOfView; }
		void SetFieldOfView(float fov);

		float GetNearClipPlane() const { return m_nearClipPlane; }
		void SetNearClipPlane(float nearPlane);

		float GetFarClipPlane() const { return m_farClipPlane; }
		void SetFarClipPlane(float farPlane);

		void GetViewportRect(float& outLeft, float& outTop, float& outRight, float& outBottom) const;
		float GetViewportLeft() const { return m_viewportLeft; }
		float GetViewportTop() const { return m_viewportTop; }
		float GetViewportRight() const { return m_viewportRight; }
		float GetViewportBottom() const { return m_viewportBottom; }
		void SetViewportRect(float left, float top, float right, float bottom);
		void SetViewportTop(float top);
		void SetViewportLeft(float left);
		void SetViewportRight(float right);
		void SetViewportBottom(float bottom);
		
		D3D11_VIEWPORT GetViewport() const;

		const Frustum& GetFrustum() const { return m_frustum; }

		DirectX::XMFLOAT4 GetBackgroundColor() const { return m_backgroundColor; }
		void SetBackgroundColor(const DirectX::XMFLOAT4& color) { m_backgroundColor = color; }

		RenderTexture* GetRenderTarget() const;
		void SetRenderTarget(RenderTexture* pRenderTarget) { m_renderTarget = pRenderTarget; }

		static constexpr eComponentType GetComponentType() { return eComponentType::Camera; }

		static std::vector<Camera*> GetAllCameras() { return s_allCameras; }
		static uint32_t GetAllCameraCount() { return static_cast<uint32_t>(s_allCameras.size()); }
		static Camera* GetMainCamera();

	private:
		eProjectionType m_projectionType = eProjectionType::Perspective;
				
		float m_fieldOfView = 45.0f;

		float m_nearClipPlane = 0.1f;
		float m_farClipPlane = 5000.0f;

		float m_viewportTop = 0.0f;
		float m_viewportLeft = 0.0f;
		float m_viewportRight = 1.0f;
		float m_viewportBottom = 1.0f;

		Frustum m_frustum;

		DirectX::XMFLOAT4 m_backgroundColor = {1.0f, 1.0f, 1.0f, 1.0f};
		RenderTexture* m_renderTarget = nullptr;

		std::unique_ptr<ConstantBuffer> m_cbCamera;

		static std::vector<Camera*> s_allCameras;
	};
}
