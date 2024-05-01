#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	class RenderTexture;

	enum class eLightType
	{
		Directional,
		Point,
		Spot
	};

	// 유니티의 경우 디렉셔널 라이트를 2개 이상 설정할 수 있지만 그림자를 만드는 것은 하나로 제한된다.
	// 언리얼의 경우 포워드 셰이딩의 경우 하나의 디렉셔널 라이트만 사용된다.
	class Light : public Component
	{
		DV_CLASS(Light, Component);
	
	public:
		Light(GameObject* pGameObject);
		~Light() override ;

		eLightType GetType() const { return m_Type; }
		void SetType(eLightType type) { m_Type = type; }

		DirectX::XMFLOAT3 GetColor() const { return m_Color; }
		void SetColor(float red, float green, float blue) { m_Color = { red, green, blue }; }
		void SetColor(const DirectX::XMFLOAT3& color) { m_Color = color; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

		float GetOuterAngleDegree() const { return m_OuterAngle; }
		float GetOuterAngleRadian() const { return DirectX::XMConvertToRadians(m_OuterAngle); }
		float GetInnerAngleDegree() const { return m_InnerAngle; }
		float GetInnerAngleRadian() const { return DirectX::XMConvertToRadians(m_InnerAngle); }
		void SetSpotLightAngles(float outerAngle, float innerAngle) { m_OuterAngle = outerAngle;  m_InnerAngle = innerAngle; }

		bool IsEnabled() const { return m_bEnabled; }
		void SetEnabled(bool enabled) { m_bEnabled = enabled; }

		// Texture*로 해야하나...?
		RenderTexture* GetShadowMap() const { return m_pShadowMap; }

		DirectX::XMMATRIX GetViewMatrix();
		DirectX::XMMATRIX GetProjectionMatrix();
		DirectX::XMMATRIX GetShaodwMatrix();

		float GetShadowMapSize() const { return m_ShadowMapSize; }
		void SetShadowMapSize(float size);



	private:
		eLightType m_Type;

		DirectX::XMFLOAT3 m_Color;

		float m_Range;
		float m_OuterAngle;
		float m_InnerAngle;

		bool m_bEnabled;

		RenderTexture* m_pShadowMap;
		float m_ShadowMapSize;
	};
}