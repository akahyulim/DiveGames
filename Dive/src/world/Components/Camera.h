#pragma once
#include "Component.h"

namespace Dive
{
	class RenderTexture;
	class ConstantBuffer;

	class Camera : public Component
	{
	public:
		// 이건 왜 또 안에 넣어놨냐...
		enum class eProjectionType
		{
			Perspective,
			Orthographic
		};

		DV_CLASS(Camera, Component)

	public:
		Camera(GameObject* gameObject);
		~Camera() override;

		void Update() override;

		// 왜 이래 놨을까?
		DirectX::XMFLOAT3 GetPosition();
		DirectX::XMMATRIX GetSceneMatrix();
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

		
		std::shared_ptr<RenderTexture> GetRenderTexture() const { return m_RenderTexture; }
		void SetRenderTexture(std::shared_ptr<RenderTexture> renderTexture) { m_RenderTexture = renderTexture; }
		DirectX::XMUINT2 GetRenderTextureSize() const;

	protected:
		eProjectionType m_ProjectionType = eProjectionType::Perspective;

		float m_FieldOfView = 45.0f;
		float m_NearClipPlane = 0.1f;
		float m_FarClipPlane = 1000.0f;
		float m_MoveSpeed = 10.0f;
		float m_RotateSpeed = 50.0f;

		DirectX::XMFLOAT4 m_BackgroundColor = {1.0f, 1.0f, 1.0f, 1.0f};
		std::shared_ptr<RenderTexture> m_RenderTexture;
	};
}