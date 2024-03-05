#pragma once
#include "Component.h"
#include "Renderer/RendererDefs.h"

namespace Dive
{
	class RenderTexture;

	class Camera : public Component
	{
	public:
		enum class eProjectionType
		{
			Perspective,
			Orthographic
		};

		DV_OBJECT(Camera, Component);

	public:
		Camera(GameObject* pGameObject);
		~Camera() override;

		DirectX::XMFLOAT3 GetPosition();
		DirectX::XMMATRIX GetWorldMatrix();
		DirectX::XMMATRIX GetViewMatrix();
		DirectX::XMMATRIX GetProjectionMatrix() const;
		DirectX::XMMATRIX GetOrthographicProjMatrix() const;
		DirectX::XMMATRIX GetPerspectiveProjMatrix() const;

		eProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(eProjectionType type) { m_ProjectionType = type; }

		const DirectX::XMFLOAT4& GetBackgroundColor() const { return m_BackgroundColor; }
		void SetBackgroundColor(float r, float g, float b, float a);

		float GetAspectRatio() const;

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float fov);

		float GetNearClipPlane() const { return m_NearClipPlane; }
		void SetNearClipPlane(float nearPlane) { m_NearClipPlane = nearPlane; }

		float GetFarClipPlane() const { return m_FarClipPlane; }
		void SetFarClipPlane(float farPlane) { m_FarClipPlane = farPlane; }

		// 뷰포트라기보단 비율을 설정할 수 있었던 것인데
		// 어느샌가 기능이 누락된 듯 하다.
		const RECT& GetViewport() const { return m_Viewport; }
		void SetViewport(const RECT& rt) { m_Viewport = rt; }

		float GetMoveSpeed() const { return m_MoveSpeed; }
		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

		float GetRotateSpeed() const { return m_RotateSpeed; }
		void SetRotateSpeed(float speed) { m_RotateSpeed = speed; }

		ID3D11RenderTargetView* GetRenderTargetView() const;
		RenderTexture* GetRenderTarget() const { return m_pRenderTarget; }
		void SetRenderTarget(RenderTexture* pRenderTarget) { m_pRenderTarget = pRenderTarget; }
		DirectX::XMFLOAT2 GetRenderTargetSize() const;

	protected:
		eProjectionType m_ProjectionType;

		DirectX::XMFLOAT4 m_BackgroundColor;
		//float m_BackgroundColor[4];

		float m_FieldOfView;

		float m_NearClipPlane;
		float m_FarClipPlane;

		float m_MoveSpeed;
		float m_RotateSpeed;

		RECT m_Viewport;

		RenderTexture* m_pRenderTarget;
	};
}