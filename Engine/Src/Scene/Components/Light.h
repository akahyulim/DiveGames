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

	// ����Ƽ�� ��� �𷺼ų� ����Ʈ�� 2�� �̻� ������ �� ������ �׸��ڸ� ����� ���� �ϳ��� ���ѵȴ�.
	// �𸮾��� ��� ������ ���̵��� ��� �ϳ��� �𷺼ų� ����Ʈ�� ���ȴ�.
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

		// Texture*�� �ؾ��ϳ�...?
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