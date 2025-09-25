#pragma once
#include "Component.h"
#include "math/Frustum.h"

namespace Dive
{
	class RenderTexture;
	class ConstantBuffer;

	enum class eRenderingPath
	{
		Forward,
		Deferred,
	};

	enum class eProjectionType
	{
		Perspective,
		Orthographic
	};

	struct alignas(16) CameraData
	{
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projMatrix;
		DirectX::XMFLOAT4X4 viewProjMatrix;
		DirectX::XMFLOAT4 cameraPosition;
		DirectX::XMFLOAT4 backgroundColor;
	};

	struct Ray
	{
		Ray(const DirectX::XMFLOAT3& org, const DirectX::XMFLOAT3 dir)
			: origin(org)
			, direction(dir)
		{}

		DirectX::XMFLOAT3 origin;
		DirectX::XMFLOAT3 direction;
	};

	struct RaycastHit
	{
		GameObject* hitObject;
		DirectX::XMFLOAT3 point;
		DirectX::XMFLOAT3 normal;
		float distance;
	};

	// https://docs.unity3d.com/ScriptReference/Camera.html
	class Camera : public Component
	{
	public:
		Camera(GameObject* gameObject);
		~Camera()override;

		void Update() override;

		void Bind();

		DirectX::XMFLOAT4X4 GetView() const;
		DirectX::XMMATRIX GetViewMatrix() const;
		DirectX::XMFLOAT4X4 GetProjection() const;
		DirectX::XMMATRIX GetProjectionMatrix() const;

		eProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(eProjectionType type) { m_ProjectionType = type; }
		
		float GetAspectRatio() const;

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float fov);

		float GetNearClipPlane() const { return m_NearClip; }
		void SetNearClipPlane(float nearPlane);

		float GetFarClipPlane() const { return m_FarClip; }
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
		
		void SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
		D3D11_VIEWPORT GetViewport() const;

		const Frustum& GetFrustum() const { return m_Frustum; }

		const float* GetBackgroundColor() const { return m_BackgroundColor; }
		DirectX::XMFLOAT4 GetBackgroundColorByXMFLOAT4() const;
		void SetBackgroundColor(float r, float g, float b, float a);
		void SetBackgroundColor(const float* color);
		void SetBackgroundColor(const DirectX::XMFLOAT4& color);

		RenderTexture* GetRenderTarget() const;
		void SetRenderTarget(RenderTexture* renderTarget);

		Ray ScreenPointToRay(const DirectX::XMUINT2& mousePosition) const;
		Ray ScreenPointToRay(uint32_t mousePositionX, uint32_t mousePositionY) const;

		static constexpr eComponentType GetComponentType() { return eComponentType::Camera; }

		static std::vector<Camera*> GetAllCameras() { return s_allCameras; }
		static uint32_t GetAllCameraCount() { return static_cast<uint32_t>(s_allCameras.size()); }
		static Camera* GetMainCamera();

	private:
		eProjectionType m_ProjectionType = eProjectionType::Perspective;
				
		float m_FieldOfView = 45.0f;

		float m_NearClip = 0.1f;
		float m_FarClip = 5000.0f;

		float m_ViewportTop = 0.0f;
		float m_ViewportLeft = 0.0f;
		float m_ViewportRight = 1.0f;
		float m_ViewportBottom = 1.0f;

		D3D11_VIEWPORT m_Viewport;

		Frustum m_Frustum;

		float m_BackgroundColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		RenderTexture* m_RenderTarget = nullptr;

		std::unique_ptr<ConstantBuffer> m_cbCamera;

		static std::vector<Camera*> s_allCameras;
	};
}
