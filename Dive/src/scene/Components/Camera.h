#pragma once
#include "Component.h"
//#include "Renderer/Renderer.h"

namespace Dive
{
	class Texture2D;
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
		Camera(GameObject* pGameObject);
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

		ID3D11RenderTargetView* GetRenderTargetView() const;
		Texture2D* GetRenderTargetTex() const { return m_pRenderTargetTex; }
		void SetRenderTargetTex(Texture2D* pRenderTarget) { m_pRenderTargetTex = pRenderTarget; }
		DirectX::XMFLOAT2 GetRenderTargetSize() const;


		// 위치 옮기기?
		ConstantBuffer* GetConstantBufferVS() override;// { return m_pCBufferVS; }
		ConstantBuffer* GetConstantBufferDS() override;// { return m_pCBufferDS; }
		ConstantBuffer* GetConstantBufferPS() override;// { return m_pCBufferPS; }

	protected:
		eProjectionType m_ProjectionType;

		DirectX::XMFLOAT4 m_BackgroundColor;

		float m_FieldOfView;

		float m_NearClipPlane;
		float m_FarClipPlane;

		float m_MoveSpeed;
		float m_RotateSpeed;

		Texture2D* m_pRenderTargetTex;

		ConstantBuffer* m_pCBufferVS;
		ConstantBuffer* m_pCBufferDS;
		ConstantBuffer* m_pCBufferPS;
	};
}
