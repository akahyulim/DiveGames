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
	class DvTexture;

	class Light : public Component
	{
		DV_CLASS(Light, Component)

	public:
		Light(GameObject* pGameObject, eLightType type);
		virtual ~Light();

		ConstantBuffer* GetConstantBufferVS() override { return m_pCBufferVS; }
		ConstantBuffer* GetConstantBufferPS() override { return m_pCBufferPS; }
		
		eLightType GetType() const { return m_Type; }

		DirectX::XMFLOAT3 GetColor() const { return m_Color; }
		void SetColor(float r, float g, float b) { m_Color = { r, g, b }; }

		DvTexture* GetShadowMap() const { return m_pShadowMap; }

		// 가로, 세로 크기가 다를 수도 있지 않을까?
		float GetShadowMapSize() const { return m_ShadowMapSize; }

		virtual DirectX::XMMATRIX GetShadowViewProj();

		bool IsShadowEnabled() const { return m_bShadowEnabled; }
		void SetShadowEnabled(bool enabled) { m_bShadowEnabled = enabled; }

	protected:
		eLightType m_Type;

		DirectX::XMFLOAT3 m_Color;

		DvTexture* m_pShadowMap;
		float m_ShadowMapSize;

		bool m_bShadowEnabled;

		ConstantBuffer* m_pCBufferVS;
		ConstantBuffer* m_pCBufferPS;
	};

	class CascadedMatrixSet
	{
	public:
		CascadedMatrixSet();
		~CascadedMatrixSet() = default;
		
		void Initialize(uint32_t size, GameObject* pCamera);
		void Update(const DirectX::XMVECTOR& dir);

		DirectX::XMMATRIX GetWorldToShadowSpace() const { return DirectX::XMLoadFloat4x4(&m_WorldToShadowSpace); }
		DirectX::XMMATRIX GetWorldToCascadeProj(uint32_t index) const { return DirectX::XMLoadFloat4x4(&m_ArrWorldToCascadeProj[index]); }
		DirectX::XMVECTOR GetToCascadeOffsetX() const { return DirectX::XMVectorSet(m_ToCascadeOffsetX[0], m_ToCascadeOffsetX[1], m_ToCascadeOffsetX[2], m_ToCascadeOffsetX[3]); }
		DirectX::XMVECTOR GetToCascadeOffsetY() const { return DirectX::XMVectorSet(m_ToCascadeOffsetY[0], m_ToCascadeOffsetY[1], m_ToCascadeOffsetY[2], m_ToCascadeOffsetY[3]); }
		DirectX::XMVECTOR GetToCascadeScale() const { return DirectX::XMVectorSet(m_ToCascadeScale[0], m_ToCascadeScale[1], m_ToCascadeScale[2], m_ToCascadeScale[3]); }

	private:
		void extractFrustumPoints(float nearClip, float farClip, DirectX::XMVECTOR* pArrFrustumCorners);
		void extractFrustumBoundSphere(float nearClip, float farClip, DirectX::XMVECTOR& boundCenter, float& boundRadius);

		bool cascadeNeedsUpdate(const DirectX::XMMATRIX& shadowView, int cascadeIndex, const DirectX::XMVECTOR& newCenter, DirectX::XMVECTOR& offset);

	private:
		uint32_t m_MapSize;
		float m_CascadeTotalRange;
		float m_ArrCascadeRagnes[4];

		DirectX::XMFLOAT3 m_ShadowBoundCenter;
		float m_ShadowBoundRadius;
		DirectX::XMFLOAT3 m_ArrCascadeBoundCenter[3];
		float m_ArrCascadeBoundRadius[3];

		DirectX::XMFLOAT4X4 m_WorldToShadowSpace;
		DirectX::XMFLOAT4X4 m_ArrWorldToCascadeProj[3];

		float m_ToCascadeOffsetX[4];
		float m_ToCascadeOffsetY[4];
		float m_ToCascadeScale[4];

		GameObject* m_pCamera;
	};

	// 유니티의 경우 디렉셔널 라이트를 2개 이상 설정할 수 있지만 그림자를 만드는 것은 하나로 제한된다.
	// 언리얼의 경우 포워드 셰이딩의 경우 하나의 디렉셔널 라이트만 사용된다.
	class DirectionalLight : public Light
	{
		DV_CLASS(DirectionalLight, Light)
			
	public:
		DirectionalLight();
		DirectionalLight(GameObject* pGameObject);
		virtual ~DirectionalLight() = default;

		void InitializeCascadedMatrixSet(GameObject* pCamera);

		void Update() override;

		ConstantBuffer* GetConstantBufferGS() override { return m_pCBufferGS; }

	private:
		CascadedMatrixSet m_CascadedMatrixSet;

		ConstantBuffer* m_pCBufferGS;
	};

	class PointLight : public Light
	{
		DV_CLASS(PointLight, Light)

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

	class SpotLight : public Light
	{
		DV_CLASS(SpotLight, Light)

	public:
		SpotLight();
		SpotLight(GameObject* pGameObject);
		virtual ~SpotLight();

		void Update() override;
 
		ConstantBuffer* GetConstantBufferDS() override { return m_pCBufferDS; }

		// GetViewProjMatrix()가 더 어울린다.
		DirectX::XMMATRIX GetShadowViewProj() override;

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

		float GetInnerAngle() const { return m_InnerAngle; }
		void SetInnerAngle(float angle) { m_InnerAngle = angle; }

		float GetOuterAngle() const { return m_OuterAngle; }
		void SetOuterAngle(float angle) { m_OuterAngle = angle; }

		void SetAngles(float inner, float outer) { m_InnerAngle = inner; m_OuterAngle = outer; }

	private:
	private:
		ConstantBuffer* m_pCBufferDS;	// m_pDomainCBuffer로 변경

		float m_Range;
		float m_InnerAngle;
		float m_OuterAngle;
	};
}