#pragma once
#include "Component.h"
#include "Renderer/Renderer.h"

namespace Dive
{
	enum class eLightType
	{
		Directional,
		Point,
		Spot
	};

	class GameObject;
	class RenderTexture;

	class DvLight : public Component
	{
		DV_CLASS(DvLight, Component)

	public:
		DvLight(GameObject* pGameObject, eLightType type);
		virtual ~DvLight();

		ConstantBuffer* GetConstantBufferVS() override { return m_pCBufferVS; }
		ConstantBuffer* GetConstantBufferPS() override { return m_pCBufferPS; }
		
		eLightType GetType() const { return m_Type; }

		DirectX::XMFLOAT3 GetColor() const { return m_Color; }
		void SetColor(float r, float g, float b) { m_Color = { r, g, b }; }

		bool IsShadowEnabled() const { return m_bShadowEnabled; }
		void SetShadowEnabled(bool enabled) { m_bShadowEnabled = enabled; }

	protected:
		eLightType m_Type;

		DirectX::XMFLOAT3 m_Color;

		//RenderTexture* m_pShadowMap;
		//float m_ShadowMapSize;

		bool m_bShadowEnabled;

		ConstantBuffer* m_pCBufferVS;
		ConstantBuffer* m_pCBufferPS;
	};

	// 유니티의 경우 디렉셔널 라이트를 2개 이상 설정할 수 있지만 그림자를 만드는 것은 하나로 제한된다.
	// 언리얼의 경우 포워드 셰이딩의 경우 하나의 디렉셔널 라이트만 사용된다.
	class DirectionalLight : public DvLight
	{
		DV_CLASS(DirectionalLight, DvLight)
			
	public:
		DirectionalLight();
		DirectionalLight(GameObject* pGameObject);
		virtual ~DirectionalLight() = default;

		void Update() override;
	};

	class PointLight : public DvLight
	{
		DV_CLASS(PointLight, DvLight)

	public:
		PointLight();
		PointLight(GameObject* pGameObject);
		virtual ~PointLight();

		void Update() override;

		ConstantBuffer* GetConstantBufferDS() override { return m_pCBufferDS; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

	private:
		ConstantBuffer* m_pCBufferDS;

		float m_Range;
	};

	class SpotLight : public DvLight
	{
		DV_CLASS(SpotLight, DvLight)

	public:
		SpotLight();
		SpotLight(GameObject* pGameObject);
		virtual ~SpotLight();

		void Update() override;
 
		ConstantBuffer* GetConstantBufferDS() override { return m_pCBufferDS; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

		float GetInnerAngle() const { return m_InnerAngle; }
		void SetInnerAngle(float angle) { m_InnerAngle = angle; }

		float GetOutterAngle() const { return m_OutterAngle; }
		void SetOutterAngle(float angle) { m_OutterAngle = angle; }

		void SetAngles(float inner, float outter) { m_InnerAngle = inner; m_OutterAngle = outter; }

	private:
	private:
		ConstantBuffer* m_pCBufferDS;

		float m_Range;
		float m_InnerAngle;
		float m_OutterAngle;
	};

	//==================================================================================================

	class Light : public Component
	{
		DV_CLASS(Light, Component)
	
	public:
		Light(GameObject* pGameObject);
		~Light() override ;

		void Update() override;

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
		DirectX::XMMATRIX GetShadowMatrix();

		float GetShadowMapSize() const { return m_ShadowMapSize; }
		void SetShadowMapSize(float size);

		bool IsShadowEnabled() const { return m_bShadowEnabled; }
		void EnableShadow(bool enable) { m_bShadowEnabled = enable; }

	private:
		eLightType m_Type;

		DirectX::XMFLOAT3 m_Color;

		float m_Range;
		float m_OuterAngle;
		float m_InnerAngle;

		bool m_bEnabled;

		RenderTexture* m_pShadowMap;
		float m_ShadowMapSize;

		bool m_bShadowEnabled;
	};
}