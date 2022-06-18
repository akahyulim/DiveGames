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
		COMPONENT_CLASS_TYPE(Light)

	public:
		Light(GameObject* pGameObject);
		~Light();

		eLightType GetLightType() const { return m_LightType; }
		void SetLightType(eLightType type) { m_LightType = type; }

		DirectX::XMFLOAT3 GetColor() const { return m_Color; }
		void SetColor(const DirectX::XMFLOAT3& color) { m_Color = color; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

		float GetSpotAngle() const { return m_SpotAngle; }
		void SetSpotAngle(float angle) { m_SpotAngle = angle; }
		float GetSpotAngleRadian() const { return DirectX::XMConvertToRadians(m_SpotAngle); }

		// ��� component���� behavior�� ���� enable�� �����Ѵ�.
		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

	private:

	private:
		eLightType m_LightType = eLightType::Directional;

		DirectX::XMFLOAT3 m_Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		float m_Range = 10.0f;

		// å�� inner�� outter�� ���еǾ� �ִ�.
		float m_SpotAngle = 60;

		// point, spot�� 1.0f, directional�� 0.5�� default����. 
		float m_Intensity = 1.0f;

		// ���� �и�?
		// �̹� Behavior�� ����� ���� �ߴ�.
		bool m_bEnabled = true;
	};
}