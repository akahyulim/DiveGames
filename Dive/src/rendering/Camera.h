#pragma once

namespace Dive
{
	class RenderTexture;

	class Camera
	{
	public:
		enum class eProjectionType
		{
			Perspective,
			Orthographic
		};

	public:
		Camera() = default;
		Camera(const DirectX::XMFLOAT4X4& projection) : m_Projection(projection) {}
		virtual ~Camera() = default;

		void SetPerspective(float fov, float nearClip, float farClip);
		void SetOrthographic(float nearClip, float farClip);

		void SetViewportSize(UINT32 width, UINT32 height);

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float fov) { m_FieldOfView = fov; calculateProjection();}

		float GetNearClip() const { return m_NearClip; }
		void SetNearClip(float nearClip) { m_NearClip = nearClip; calculateProjection(); }

		float GetFarClip() const { return m_FarClip; }
		void SetFarClip(float farClip) { m_FarClip = farClip; calculateProjection(); }

		eProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(eProjectionType type) { m_ProjectionType = type; calculateProjection(); }

		std::shared_ptr<RenderTexture> GetRenderTarget() const { return m_RenderTexture; }
		void SetRenderTarget(std::shared_ptr<RenderTexture> renderTarget) { m_RenderTexture = renderTarget; }

		void GetBackgroundColor(DirectX::XMFLOAT4& color) { color = m_BackgroundColor; }
		void SetBackgroundColor(const DirectX::XMFLOAT4& color) { m_BackgroundColor = color; }
		void SetBackgroundColor(float r, float g, float b, float a) { m_BackgroundColor = { r, g, b, a }; }

	private:
		void calculateProjection();

	private:
		DirectX::XMFLOAT4X4 m_Projection = { 1.0f, 0.0f, 0.0f, 0.0f,
									 0.0f, 1.0f, 0.0f, 0.0f,
									 0.0f, 0.0f, 1.0f, 0.0f,
									 0.0f, 0.0f, 0.0f, 1.0f };

		eProjectionType m_ProjectionType = eProjectionType::Perspective;

		float m_AspectRatio = 0.0f;
		float m_FieldOfView = 45.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;

		DirectX::XMFLOAT4 m_BackgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		std::shared_ptr<RenderTexture> m_RenderTexture;
	};	
}