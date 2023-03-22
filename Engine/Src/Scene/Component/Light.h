#pragma once
#include "Component.h"

namespace Dive
{
	enum class eLightType
	{
		Directional,
		Point,
		Spot
	};

	class Light : public Component
	{
		DIVE_OBJECT(Light, Component)
			
	public:
		explicit Light(Context* pContext);
		~Light() override;

		eLightType GetType() const { return m_Type; }
		void SetType(eLightType type) { m_Type = type; }

		DirectX::XMFLOAT3 GetDir() const { return m_Dir; }
		void SetDir(float x, float y, float z) { m_Dir = DirectX::XMFLOAT3(x, y, z); }
		void SetDir(const DirectX::XMFLOAT3& dir) { m_Dir = dir; }

		DirectX::XMFLOAT3 GetColor() const { return m_Color; }
		void SetColor(float red, float green, float blue) { m_Color = DirectX::XMFLOAT3(red, green, blue); }
		void SetColor(const DirectX::XMFLOAT3& color) { m_Color = color; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

		float GetSpotAngle() const { return m_SpotAngle; }
		void SetSpotAngle(float angle) { m_SpotAngle = angle; }
		float GetSpotAngleRadian() const { return DirectX::XMConvertToRadians(m_SpotAngle); }

		// 몇몇 component들은 behavior을 통해 enable을 관리한다.
		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

	private:
	private:
		eLightType m_Type;

		DirectX::XMFLOAT3 m_Dir;
		DirectX::XMFLOAT3 m_Color;

		float m_Range;
		float m_SpotAngle;
		float m_Intensity;

		// behavior을 만들건가?
		bool m_bEnabled;
	};
}