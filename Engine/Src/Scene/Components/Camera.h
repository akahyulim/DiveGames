#pragma once
#include "Component.h"
#include "Renderer/RendererDefs.h"

namespace Dive
{
	class Skydome;

	class Camera : public Component
	{
	public:
		Camera(GameObject* pGameObject);
		~Camera() override;

		bool IsInViewFrustum(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& extents) const;

		DirectX::XMMATRIX GetWorldMatrix() const;

		// 유니티에서는 WorldToCameraMatrix
		// 반대인 CameraToWorldMatrix도 존재한다.
		DirectX::XMMATRIX GetViewMatrix() const;

		// 이건 유니티에서 ProjectionMatrix
		DirectX::XMMATRIX GetProjectionMatrix() const;
		DirectX::XMMATRIX GetOrthographicProjMatrix() const;
		DirectX::XMMATRIX GetPerspectiveProjMatrix() const;

		float GetViewWidth() const { return m_ViewWidth; }
		float GetViewHeight() const { return m_ViewHeight; }
		void SetViewSize(float width, float height) { m_ViewWidth = width; m_ViewHeight = height; }

		DirectX::XMFLOAT4 GetBackgroundColor() const { return m_BackgroundColor; }
		void SetBackgroundColor(DirectX::XMFLOAT4 color) { m_BackgroundColor = color; }

		bool IsOrthographic() const { return m_bOrthographic; }
		void SetOrthographic(bool orthographic) { m_bOrthographic = orthographic; }

		float GetAspectRatio() const;

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float fov);

		float GetNearClipPlane() const { return m_NearClipPlane; }
		void SetNearClipPlane(float nearPlane) { m_NearClipPlane = nearPlane; }

		float GetFarClipPlane() const { return m_FarClipPlane; }
		void SetFarClipPlane(float farPlane) { m_FarClipPlane = farPlane; }

		void GetViewportRectRate(float& outX, float& outY, float& outWidth, float& outHeight);
		void SetViewportRectRate(float x, float y, float width, float height);
		D3D11_VIEWPORT GetViewport();

		eRenderingPath GetRenderingPath() const { return m_RenderingPath; }
		void SetRenderingPath(eRenderingPath path) { m_RenderingPath = path; }

		float GetMoveSpeed() const { return m_MoveSpeed; }
		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

		float GetRotateSpeed() const { return m_RotateSpeed; }
		void SetRotateSpeed(float speed) { m_RotateSpeed = speed; }

		// temp : 유니티를 따라하긴 했는데 굳이 여기에서 관리해야 하나...?
		// Renderer 혹은 RendererSettings 같은걸 만들자.
		void SetSkydome(Skydome* pSkydome) { m_pSkydome = pSkydome; }
		Skydome* GetSkydome() const { return m_pSkydome; }

		// 유니티 api에는 특정 포인트를 특정 좌표계로 계산해주는 함수들이 있다.
		// ScreenPointToRay
		// ScreenToWorldPoint
		// ScreenToViewportPoint
		// ViewportPointToRay
		// ViewportToScreenPoint
		// ViewportToWorldPoint
		// WorldToScreenPoint
		// WorldToViewportPoint

	private: 
	protected:
		bool m_bOrthographic;
		
		float m_ViewWidth;
		float m_ViewHeight;

		DirectX::XMFLOAT4 m_BackgroundColor;

		float m_FieldOfView;

		float m_NearClipPlane;
		float m_FarClipPlane;

		float m_ViewportRectRateX;
		float m_ViewportRectRateY;
		float m_ViewportRectRateWidth;
		float m_ViewportRectRateHeight;

		float m_MoveSpeed;
		float m_RotateSpeed;

		eRenderingPath m_RenderingPath;

		Skydome* m_pSkydome;
	};
}