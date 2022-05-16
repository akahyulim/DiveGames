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
	public:
		COMPONENT_CLASS_TYPE(Light)

		Light(GameObject* pGameObject, unsigned long long id = 0);
		~Light();

		eLightType GetLightType() const { return m_LightType; }
		void SetLightType(eLightType type) { m_LightType = type; }

		DirectX::XMFLOAT3 GetColor() const { return m_Color; }
		void SetColor(const DirectX::XMFLOAT3& color) { m_Color = color; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

		// 몇몇 component들은 behavior을 통해 enable을 관리한다.
		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

	private:

	private:
		eLightType m_LightType = eLightType::Directional;

		DirectX::XMFLOAT3 m_Color		= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		float m_Range = 10.0f;

		// 추후 분리?
		bool m_bEnabled = true;
	};
}