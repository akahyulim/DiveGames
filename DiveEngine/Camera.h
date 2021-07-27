#pragma once
#include "Component.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include <DirectXMath.h>

namespace dive
{
	class GameObject;

	enum class eProjectionType
	{
		Perspective,
		Orthographic,
	};

	// �� ������ �Է��� �޾� Transform�� ���ѽ�Ű��
	// �̸� �̿��Ͽ� View, Projection Matrix�� ����Ѵ�.
	// �̿ܿ��� �ø�, ��ŷ ���� ����� �����Ѵ�.
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
		
		Vector4 GetClearClor() const { return m_ClearColor; }
		void SetClearColor(const Vector4& color) { m_ClearColor = color; }
		void SetClearColor(float r, float g, float b, float a) { m_ClearColor = Vector4(r, g, b, a); }

	private:
		DirectX::XMMATRIX computeViewMatrix() const;

	private:
		eProjectionType m_ProjectionType;

		float m_NearPlane;
		float m_FarPlane;

		Vector3 m_Position;
		Vector4 m_ClearColor;
		Matrix4x4 m_View;
		Matrix4x4 m_Projection;
	};
}