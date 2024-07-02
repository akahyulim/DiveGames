#pragma once
#include "Component.h"
#include "Renderer/Renderer.h"

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

		DV_CLASS(Camera, Component);

	public:
		Camera(GameObject* pGameObject);
		~Camera() override;

		void Update() override;

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

		float GetMoveSpeed() const { return m_MoveSpeed; }
		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

		float GetRotateSpeed() const { return m_RotateSpeed; }
		void SetRotateSpeed(float speed) { m_RotateSpeed = speed; }

		ID3D11RenderTargetView* GetRenderTargetView() const;
		RenderTexture* GetRenderTarget() const { return m_pRenderTarget; }
		void SetRenderTarget(RenderTexture* pRenderTarget) { m_pRenderTarget = pRenderTarget; }
		DirectX::XMFLOAT2 GetRenderTargetSize() const;

		const VSConstBuf_Camera& GetCBufferVS() const { return m_CBufferVS; }
		const PSConstBuf_Camera& GetCBufferPS() const { return m_CBufferPS; }

	protected:
		eProjectionType m_ProjectionType;

		DirectX::XMFLOAT4 m_BackgroundColor;

		float m_FieldOfView;

		float m_NearClipPlane;
		float m_FarClipPlane;

		float m_MoveSpeed;
		float m_RotateSpeed;

		RenderTexture* m_pRenderTarget;

		VSConstBuf_Camera m_CBufferVS;
		PSConstBuf_Camera m_CBufferPS;
	};
}