#pragma once
#include "Component.h"
#include "Renderer/RendererDefs.h"

namespace Dive
{
	class Skydome;

	class Camera : public Component
	{
	public:
		enum class eProjectionType
		{
			Perspective,
			Orthographic
		};

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

		DirectX::XMFLOAT4 GetBackgroundColor() const { return m_BackgroundColor; }
		void SetBackgroundColor(float r, float g, float b, float a) { m_BackgroundColor = { r, g, b, a }; }
		void SetBackgroundColor(DirectX::XMFLOAT4 color) { m_BackgroundColor = color; }

		float GetAspectRatio() const;

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float fov);

		float GetNearClipPlane() const { return m_NearClipPlane; }
		void SetNearClipPlane(float nearPlane) { m_NearClipPlane = nearPlane; }

		float GetFarClipPlane() const { return m_FarClipPlane; }
		void SetFarClipPlane(float farPlane) { m_FarClipPlane = farPlane; }

		D3D11_VIEWPORT GetViewport() const;
		void GetViewportRectRate(float& outX, float& outY, float& outWidth, float& outHeight);
		void SetViewportRectRate(float x, float y, float width, float height);
		
		float GetMoveSpeed() const { return m_MoveSpeed; }
		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

		float GetRotateSpeed() const { return m_RotateSpeed; }
		void SetRotateSpeed(float speed) { m_RotateSpeed = speed; }

		void SetSkydome(Skydome* pSkydome) { m_pSkydome = pSkydome; }
		Skydome* GetSkydome() const { return m_pSkydome; }

	protected:
		eProjectionType m_ProjectionType;
	
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

		// 일단 외부에서 생성 후 전달받아 관리
		Skydome* m_pSkydome;
	};
}