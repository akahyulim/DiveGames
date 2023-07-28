#pragma once
#include "Component.h"

namespace Dive
{
	class Camera : public Component
	{
	public:
		Camera(GameObject* pGameObject);
		~Camera() override;

		DirectX::XMMATRIX GetViewMatrix() const;

		DirectX::XMMATRIX GetProjectionMatrix() const;
		DirectX::XMMATRIX GetOrthographicProjMatrix() const;
		DirectX::XMMATRIX GetPerspectiveProjMatrix() const;

		float GetViewWidth() const { return m_ViewWidth; }
		float GetViewHeight() const { return m_ViewHeight; }
		void SetViewSize(float width, float height) { m_ViewWidth = width; m_ViewHeight = height; }

		bool IsOrthographic() const { return m_bOrthographic; }
		void SetOrthographic(bool orthographic) { m_bOrthographic = orthographic; }

		float GetAspectRatio() const;

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float fov);

		float GetNearClipPlane() const { return m_NearClipPlane; }
		void SetNearClipPlane(float nearPlane) { m_NearClipPlane = nearPlane; }

		float GetFarClipPlane() const { return m_FarClipPlane; }
		void SetFarClipPlane(float farPlane) { m_FarClipPlane = farPlane; }

		void GetViewportRect(float& x, float& y, float& w, float& h);
		void SetViewportRect(float x, float y, float w, float h);
		D3D11_VIEWPORT GetViewport();

	private:
	private:
		bool m_bOrthographic;
		
		float m_ViewWidth;
		float m_ViewHeight;

		float m_FieldOfView;

		float m_NearClipPlane;
		float m_FarClipPlane;

		float m_ViewportRectX;
		float m_ViewportRectY;
		float m_ViewportRectW;
		float m_ViewportRectH;
	};
}