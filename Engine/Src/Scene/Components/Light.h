#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	enum class eLightType
	{
		Directional,
		Point,
		Spot
	};

	class Light : public Component
	{
		DV_OBJECT(Light, Component);
	
	public:
		Light(GameObject* pGameObject);
		~Light() override ;

		eLightType GetType() const { return m_Type; }
		void SetType(eLightType type) { m_Type = type; }

		DirectX::XMFLOAT3 GetLookAt() const { return m_LookAt; }
		void SetLookAt(float x, float y, float z) { m_LookAt = { x, y,z }; }
		void SetLookAt(DirectX::XMFLOAT3 lookAt) { m_LookAt = lookAt; }
		DirectX::XMFLOAT3 GetDirection();

		DirectX::XMFLOAT3 GetColor() const { return m_Color; }
		void SetColor(float red, float green, float blue) { m_Color = { red, green, blue }; }
		void SetColor(const DirectX::XMFLOAT3& color) { m_Color = color; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

		float GetOuterAngleDegree() const { return m_OuterAngle; }
		float GetOuterAngleRadian() const { return DirectX::XMConvertToRadians(m_OuterAngle); }
		float GetInnerAngleDegree() const { return m_InnerAngle; }
		float GetInnerAngleRadian() const { return DirectX::XMConvertToRadians(m_InnerAngle); }
		void SetSpotLightAngles(float cosOuterCone, float innerAngle) { m_OuterAngle = cosOuterCone;  m_InnerAngle = innerAngle; }

		bool IsEnabled() const { return m_bEnabled; }
		void SetEnabled(bool enabled) { m_bEnabled = enabled; }

	private:
		eLightType m_Type;

		DirectX::XMFLOAT3 m_LookAt;
		DirectX::XMFLOAT3 m_Color;

		float m_Range;
		float m_OuterAngle;
		float m_InnerAngle;

		bool m_bEnabled;
	};
}