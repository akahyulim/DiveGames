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

		bool IsOrthographic() const { return m_bOrthographic; }
		void SetOrthographic(bool orthographic) { m_bOrthographic = orthographic; }
		void SetOrthoViewSize(float size);
		void SetOrthoViewSize(const DirectX::XMFLOAT2& size);

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspect) { m_AspectRatio = aspect; }
		void SetAspectRatio(float width, float height) { m_AspectRatio = width / height; }

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float fov);

		float GetNearClipPlane() const { return m_NearClipPlane; }
		void SetNearClipPlane(float nearPlane) { m_NearClipPlane = nearPlane; }

		float GetFarClipPlane() const { return m_FarClipPlane; }
		void SetFarClipPlane(float farPlane) { m_FarClipPlane = farPlane; }

	private:
	private:
		bool m_bOrthographic;
		DirectX::XMFLOAT2 m_OrthoViewSize;

		float m_FieldOfView;
		float m_AspectRatio;
		float m_NearClipPlane;
		float m_FarClipPlane;
	};
}