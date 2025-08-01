#pragma once
#include "Component.h"
#include "math/Frustum.h"

namespace Dive
{
	class RenderTexture;

	enum class eProjectionType
	{
		Perspective,
		Orthographic
	};

	// https://docs.unity3d.com/ScriptReference/Camera.html
	class Camera : public Component
	{
	public:
		Camera(GameObject* gameObject);
		~Camera()override;

		void Update() override;

		void Bind(ID3D11DeviceContext* deviceContext) const;

		DirectX::XMFLOAT4X4 GetView() const;
		DirectX::XMMATRIX GetViewMatrix() const;
		DirectX::XMFLOAT4X4 GetProjection() const;
		DirectX::XMMATRIX GetProjectionMatrix() const;

		eProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(eProjectionType type) { m_ProjectionType = type; }
		
		float GetAspectRatio() const;

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float fov);

		float GetNearClipPlane() const { return m_NearClipPlane; }
		void SetNearClipPlane(float nearPlane);

		float GetFarClipPlane() const { return m_FarClipPlane; }
		void SetFarClipPlane(float farPlane);

		void GetViewportRect(float& outLeft, float& outTop, float& outRight, float& outBottom) const;
		float GetViewportLeft() const { return m_ViewportLeft; }
		float GetViewportTop() const { return m_ViewportTop; }
		float GetViewportRight() const { return m_ViewportRight; }
		float GetViewportBottom() const { return m_ViewportBottom; }
		void SetViewportRect(float left, float top, float right, float bottom);
		void SetViewportTop(float top);
		void SetViewportLeft(float left);
		void SetViewportRight(float right);
		void SetViewportBottom(float bottom);
		
		D3D11_VIEWPORT GetViewport() const;

		const Frustum& GetFrustum() const { return m_Frustum; }

		DirectX::XMFLOAT4 GetBackgroundColor() const { return m_BackgroundColor; }
		void SetBackgroundColor(const DirectX::XMFLOAT4& color) { m_BackgroundColor = color; }

		RenderTexture* GetRenderTarget() const;
		void SetRenderTarget(RenderTexture* pRenderTarget) { m_RenderTarget = pRenderTarget; }

		static constexpr eComponentType GetType() { return eComponentType::Camera; }

		static std::vector<Camera*> GetAllCameras() { return s_AllCameras; }
		static uint32_t GetAllCameraCount() { return static_cast<uint32_t>(s_AllCameras.size()); }
		static Camera* GetMainCamera();

	private:
		eProjectionType m_ProjectionType = eProjectionType::Perspective;
				
		float m_FieldOfView = 45.0f;

		float m_NearClipPlane = 0.1f;
		float m_FarClipPlane = 5000.0f;

		float m_ViewportTop = 0.0f;
		float m_ViewportLeft = 0.0f;
		float m_ViewportRight = 1.0f;
		float m_ViewportBottom = 1.0f;

		Frustum m_Frustum;

		DirectX::XMFLOAT4 m_BackgroundColor = {1.0f, 1.0f, 1.0f, 1.0f};
		RenderTexture* m_RenderTarget = nullptr;

		static std::vector<Camera*> s_AllCameras;
	};
}
