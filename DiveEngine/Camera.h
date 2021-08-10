#pragma once
#include "GraphicsInclude.h"
#include "Component.h"

namespace dive
{
	class GameObject;

	enum class eProjectionType
	{
		Perspective,
		Orthographic,
	};

	// 주 역할은 입력을 받아 Transform을 변한시키고
	// 이를 이용하여 View, Projection Matrix를 계산한다.
	// 이외에도 컬링, 픽킹 등의 기능을 수행한다.
	class Camera : public Component
	{
	public:
		Camera(GameObject* pGameObject);
		~Camera();

		void Update(float deltaTime) override;

		eProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(eProjectionType type);

		float GetFieldOfView() const { return m_FieldOfView; }
		void SetFieldOfView(float angle);

		float GetViewWidth() const { return m_ViewWidth; }
		void SetViewWidth(float width);

		float GetViewHeight() const { return m_ViewHeight; }
		void SetViewHeight(float height);

		float GetAspect() const { return m_ViewWidth / m_ViewHeight; };

		float GetNearPlane() const { return m_NearPlane; }
		void SetNearPlane(float nearPlane);
		float GetFarPlane() const { return m_FarPlane; }
		void SetFarPlane(float farPlane);
		
		DirectX::XMFLOAT4 GetBackgroundColor() const { return m_BackgroundColor; }
		void SetBackgroundColor(const DirectX::XMFLOAT4& color);
		void SetBackgroundColor(float r, float g, float b, float a) { SetBackgroundColor(DirectX::XMFLOAT4(r, g, b, a)); }

		DirectX::XMFLOAT4X4 GetView() const { return m_View; }
		DirectX::XMFLOAT4X4 GetProjection() const { return m_Projection; }

	private:
		DirectX::XMMATRIX computeViewMatrix() const;
		DirectX::XMMATRIX computeProjectionMatrix() const;

	private:
		eProjectionType m_ProjectionType;

		float m_FieldOfView;
		float m_ViewWidth;
		float m_ViewHeight;
		float m_NearPlane;
		float m_FarPlane;

		bool m_bUpdated;

		DirectX::XMFLOAT4 m_BackgroundColor;
		DirectX::XMFLOAT4X4 m_View;
		DirectX::XMFLOAT4X4 m_Projection;
	};
}