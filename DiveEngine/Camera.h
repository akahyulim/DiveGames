#pragma once
#include "Component.h"
#include <DirectXMath.h>

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
		void SetProjectionType(eProjectionType type) { m_ProjectionType = type; }

		float GetNearPlane() const { return m_NearPlane; }
		void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; }
		float GetFarPlane() const { return m_FarPlane; }
		void SetFarPlane(float farPlane) { m_FarPlane = farPlane; }
		
		DirectX::XMFLOAT4 GetClearClor() const { return m_ClearColor; }
		void SetClearColor(const DirectX::XMFLOAT4& color) { m_ClearColor = color; }
		void SetClearColor(float r, float g, float b, float a) { m_ClearColor = DirectX::XMFLOAT4(r, g, b, a); }

	private:
		DirectX::XMMATRIX computeViewMatrix() const;

	private:
		eProjectionType m_ProjectionType;

		float m_NearPlane;
		float m_FarPlane;

		DirectX::XMFLOAT3 m_Position;
		DirectX::XMFLOAT4 m_ClearColor;
		DirectX::XMFLOAT4X4 m_View;
		DirectX::XMFLOAT4X4 m_Projection;
	};
}