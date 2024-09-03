#include "DivePch.h"
#include "Light.h"
#include "Core/CoreDefs.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/ConstantBuffer.h"
#include "Scene/GameObject.h"

#include "Graphics/Graphics.h"

namespace Dive
{
	DvLight::DvLight(GameObject* pGameObject, eLightType type)
		: Component(pGameObject)
		, m_Type(type)
		, m_Color{1.0f, 1.0f, 1.0f}
		, m_pShadowMap(nullptr)
		, m_ShadowMapSize(0.0f)
		, m_bShadowEnabled(false)
		, m_pCBufferVS(nullptr)
		, m_pCBufferPS(nullptr)
	{
	}

	DvLight::~DvLight()
	{
		DV_DELETE(m_pShadowMap);

		DV_DELETE(m_pCBufferPS);
		DV_DELETE(m_pCBufferVS);
	}

	DirectX::XMMATRIX DvLight::GetShadowViewProj()
	{
		return DirectX::XMMatrixIdentity();
	}

#pragma pack(push, 1)
	struct CB_DIRECTIONAL_VS
	{
		// matViewProj
	};

	struct CB_DIRECTIONAL_PS
	{
		DirectX::XMFLOAT3 lightColor;
		float padding;

		DirectX::XMFLOAT3 dirToLight;
		uint32_t shadowEnabled;
	};
#pragma pack(pop)

	DirectionalLight::DirectionalLight()
		: DvLight(nullptr, eLightType::Directional)
	{
	}

	DirectionalLight::DirectionalLight(GameObject* pGameObject)
		: DvLight(pGameObject, eLightType::Directional)
	{
	}
	
	void DirectionalLight::Update()
	{
		if(!m_pCBufferPS)
			m_pCBufferPS = ConstantBuffer::Create("CB_DIRECTIONAL_PS", sizeof(CB_DIRECTIONAL_PS));

		auto pMappedData = reinterpret_cast<CB_DIRECTIONAL_PS*>(m_pCBufferPS->Map());
		pMappedData->lightColor = m_Color;
		pMappedData->dirToLight = { -1.0f * m_pGameObject->GetForward().x, -1.0f * m_pGameObject->GetForward().y, -1.0f * m_pGameObject->GetForward().z };
		pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;
		m_pCBufferPS->Unmap();
	}

#pragma pack(push, 1)
	struct CB_POINT_DS
	{
		DirectX::XMMATRIX world;
	};

	struct CB_POINT_PS
	{
		DirectX::XMFLOAT3 lightColor;
		float lightRangeRcp;

		DirectX::XMFLOAT3 lightPos;
		uint32_t shadowEnabled;
	};
#pragma pack(pop)

	PointLight::PointLight()
		: DvLight(nullptr, eLightType::Point)
		, m_pCBufferDS(nullptr)
		, m_Range(0.0f)
	{
	}

	PointLight::PointLight(GameObject* pGameObject)
		: DvLight(pGameObject, eLightType::Point)
		, m_pCBufferDS(nullptr)
		, m_Range(0.0f)
	{
	}

	PointLight::~PointLight()
	{
		DV_DELETE(m_pCBufferDS);
	}

	void PointLight::Update()
	{
		// ds constant buffer
		{
			if(!m_pCBufferDS)
				m_pCBufferDS = ConstantBuffer::Create("CB_POINT_DS", sizeof(CB_POINT_DS));
			
			auto scale = DirectX::XMMatrixScaling(m_Range, m_Range, m_Range);
			auto trans = DirectX::XMMatrixTranslation(m_pGameObject->GetPosition().x, m_pGameObject->GetPosition().y, m_pGameObject->GetPosition().z);
			auto world = DirectX::XMMatrixTranspose(scale * trans);

			auto pMappedData = reinterpret_cast<CB_POINT_DS*>(m_pCBufferDS->Map());
			pMappedData->world = world;
			m_pCBufferDS->Unmap();
		}

		// ps constant buffer
		{
			if(!m_pCBufferPS)
				m_pCBufferPS = ConstantBuffer::Create("CB_POINT_PS", sizeof(CB_POINT_PS));

			auto pMappedData = reinterpret_cast<CB_POINT_PS*>(m_pCBufferPS->Map());
			pMappedData->lightColor = m_Color;
			pMappedData->lightRangeRcp = 1 / m_Range;
			pMappedData->lightPos = m_pGameObject->GetPosition();
			pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;
			m_pCBufferPS->Unmap();
		}
	}

#pragma pack(push, 1)
	struct CB_SPOTLIGHT_VS
	{
		DirectX::XMMATRIX matViewProj;
	};

	struct CB_SPOTLIGHT_DS
	{
		DirectX::XMMATRIX world;

		float sinOuter;
		float cosOuter;
		float padding[2];
	};

	struct CB_SPOTLIGHT_PS
	{
		DirectX::XMFLOAT3 lightColor;
		float lightRangeRcp;

		DirectX::XMFLOAT3 lightPos;
		float cosOuterCone;

		DirectX::XMFLOAT3 dirToLight;
		float cosConeAttRange;

		DirectX::XMMATRIX matViewProj;

		uint32_t shadowEnabled;
		float padding[3];
	};
#pragma pack(pop)

	SpotLight::SpotLight()
		: DvLight(nullptr, eLightType::Spot)
		, m_pCBufferDS(nullptr)
		, m_Range(0.0f)
		, m_InnerAngle(0.0f)
		, m_OuterAngle(0.0f)
	{
		// 임시 위치
		// 기본적으로 nullptr이 맞고
		// 그림자를 적용하고자 할 때 생성하는 것이 맞는 듯 하다.
		// 다만 현재 크기가 고정되어 있다.
		m_ShadowMapSize = 1024.0f;
		m_pShadowMap = new RenderTexture();
		m_pShadowMap->CreateDepthStencilView(m_ShadowMapSize, m_ShadowMapSize, DXGI_FORMAT_R32_TYPELESS);
	}

	SpotLight::SpotLight(GameObject* pGameObject)
		: DvLight(pGameObject, eLightType::Spot)
		, m_pCBufferDS(nullptr)
		, m_Range(0.0f)
		, m_InnerAngle(0.0f)
		, m_OuterAngle(0.0f)
	{
		m_ShadowMapSize = 1024.0f;
		m_pShadowMap = new RenderTexture();
		m_pShadowMap->CreateDepthStencilView(m_ShadowMapSize, m_ShadowMapSize, DXGI_FORMAT_R32_TYPELESS);
	}

	SpotLight::~SpotLight()
	{
		DV_DELETE(m_pCBufferDS);
	}

	void SpotLight::Update()
	{
		auto matViewProj = GetShadowViewProj();

		// vs constant buffer
		{
			if (!m_pCBufferVS)
				m_pCBufferVS = ConstantBuffer::Create("CB_SPOTLIGHT_VS", sizeof(CB_SPOTLIGHT_VS));

			auto pMappedData = reinterpret_cast<CB_SPOTLIGHT_VS*>(m_pCBufferVS->Map());
			pMappedData->matViewProj = DirectX::XMMatrixTranspose(matViewProj);
			m_pCBufferVS->Unmap();
		}

		float outerRad = DirectX::XMConvertToRadians(m_OuterAngle);
		float innerRad = DirectX::XMConvertToRadians(m_InnerAngle);

		float sinOuter = sinf(outerRad);
		float cosOuter = cosf(outerRad);
		float cosInner = cosf(innerRad);

		// ds constant buffer
		{
			if (!m_pCBufferDS)
				m_pCBufferDS = ConstantBuffer::Create("CB_SPOTLIGHT_DS", sizeof(CB_SPOTLIGHT_DS));

			m_pGameObject->SetScale(m_Range, m_Range, m_Range);
			auto world = m_pGameObject->GetMatrix();

			auto pMappedData = reinterpret_cast<CB_SPOTLIGHT_DS*>(m_pCBufferDS->Map());
			pMappedData->world = DirectX::XMMatrixTranspose(world);
			pMappedData->sinOuter = sinOuter;
			pMappedData->cosOuter = cosOuter;
			m_pCBufferDS->Unmap();
		}

		// ps constant buffer
		{
			if (!m_pCBufferPS)
				m_pCBufferPS = ConstantBuffer::Create("CB_SPOTLIGHT_PS", sizeof(CB_SPOTLIGHT_PS));

			auto pMappedData = reinterpret_cast<CB_SPOTLIGHT_PS*>(m_pCBufferPS->Map());
			pMappedData->lightColor = m_Color;
			pMappedData->lightRangeRcp = 1.0f / m_Range;
			pMappedData->lightPos = m_pGameObject->GetPosition();
			pMappedData->cosOuterCone = cosOuter;
			pMappedData->dirToLight = { -1.0f * m_pGameObject->GetForward().x, -1.0f * m_pGameObject->GetForward().y, -1.0f * m_pGameObject->GetForward().z };
			pMappedData->cosConeAttRange = cosInner - cosOuter;
			pMappedData->matViewProj = DirectX::XMMatrixTranspose(matViewProj);
			pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;
			m_pCBufferPS->Unmap();
		}
	}

	DirectX::XMMATRIX SpotLight::GetShadowViewProj()
	{
		const auto& pos = m_pGameObject->GetPositionVector();
		const auto& forward = m_pGameObject->GetForwardVector();
		const auto& focus = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(forward, m_Range));

		const auto& up = m_pGameObject->GetUpwardVector();
		
		auto view = DirectX::XMMatrixLookAtLH(pos, focus, up);

		auto proj = DirectX::XMMatrixPerspectiveFovLH(
			2.0f * DirectX::XMConvertToRadians(m_OuterAngle),
			1.0f,			// 가로, 세로 비율이라 1인듯 하다.
			5.0f,			// 책에서도 상수를 멤버 변수로 관리했다. ShadowGen Rasterizer State와 관련이 있나? => 아닌듯?
			m_Range
		);	

		return DirectX::XMMatrixMultiply(view, proj);
	}

	//========================================================================================================================================================

	Light::Light(GameObject* pGameObject)
		: Component(pGameObject)
		, m_Type(eLightType::Directional)
		, m_Color(1.0f, 1.0f, 1.0f)
		, m_Range(10.0f)
		, m_OuterAngle(30.0f)
		, m_InnerAngle(15.0f)
		, m_bEnabled(true)
		, m_pShadowMap(nullptr)
		, m_ShadowMapSize(1024.0f)
		, m_bShadowEnabled(true)
	{
		m_pShadowMap = new RenderTexture();
		m_pShadowMap->CreateDepthStencilView(m_ShadowMapSize, m_ShadowMapSize, DXGI_FORMAT_R32_TYPELESS);
	}

	Light::~Light()
	{
		DV_DELETE(m_pShadowMap);

		DV_ENGINE_TRACE("컴포넌트({0:s}'s {1:s}) 소멸", GetName(), GetTypeName());
	}

	void Light::Update()
	{
		/*
		switch (GetType())
		{
		case eLightType::Directional:
		{	
			m_CBufferPS.options = (1U << 0);
			m_CBufferPS.dirToLight = m_pGameObject->GetForward();
			break;
		}
		case eLightType::Point:
		{
			{
				DirectX::XMMATRIX lightWorldScale = DirectX::XMMatrixScaling(m_Range, m_Range, m_Range);
				DirectX::XMMATRIX lightWorldTrans = DirectX::XMMatrixTranslation(m_pGameObject->GetPosition().x, m_pGameObject->GetPosition().y, m_pGameObject->GetPosition().z);
				// 아래 두 행렬은 카메라의 것이어야 한다.
				// 직접 가져오는 것이 애매하다면 위의 두 값으로 world만 계산한 후 전달하고
				// 셰이더 내부에서 카메라로 부터 전달받은 view와 proj를 곱하는 방법도 있다.
				// 물론 이 경우 카메라에도 cb_domain을 추가해야 한다.
				// 그것도 귀찮다면 어떻게든 카메라를 가져오는 수도 있다.
				DirectX::XMMATRIX view = GetViewMatrix();
				DirectX::XMMATRIX proj = GetProjectionMatrix();
				DirectX::XMMATRIX worldViewProjection = lightWorldScale * lightWorldTrans * view * proj;
				m_CBufferDS.lightProjection = DirectX::XMMatrixTranspose(worldViewProjection);
			}

			m_CBufferPS.options = (1U << 1);
			m_CBufferPS.lightPos = m_pGameObject->GetPosition();
			m_CBufferPS.lightRangeRcp = 1.0f / GetRange();
			break;
		}
		case eLightType::Spot:
		{
			m_CBufferVS.shadow = DirectX::XMMatrixTranspose(GetShadowMatrix());

			m_CBufferPS.options = (1U << 2);
			m_CBufferPS.lightPos = m_pGameObject->GetPosition();
			m_CBufferPS.dirToLight = m_pGameObject->GetForward();
			m_CBufferPS.lightRangeRcp = 1.0f / GetRange();
			m_CBufferPS.outerConeAngle = GetOuterAngleRadian();
			m_CBufferPS.innerConeAngle = GetInnerAngleRadian();
			m_CBufferPS.shadow = DirectX::XMMatrixTranspose(GetShadowMatrix());
			break;
		}
		default:
			break;
		}
		m_CBufferPS.lightColor = { GetColor().x * GetColor().x
		, GetColor().y * GetColor().y
		, GetColor().z * GetColor().z };

		m_CBufferPS.shadowEnabled = m_bShadowEnabled ? 1 : 0;
		*/
	}

	DirectX::XMMATRIX Light::GetViewMatrix()
	{
		const auto& pos = m_pGameObject->GetPositionVector();
		const auto& forward = m_pGameObject->GetForwardVector();
		const auto& up = m_pGameObject->GetUpwardVector();

		const auto& focus = DirectX::XMVectorAdd(pos, DirectX::XMVectorMultiply(forward, DirectX::XMVectorReplicate(m_Range)));

		return DirectX::XMMatrixLookAtLH(pos, focus, up);
	}

	DirectX::XMMATRIX Light::GetProjectionMatrix()
	{
		return DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(2.0f * m_OuterAngle),
			1.0f,
			5.0f,			// 책에서도 상수를 멤버 변수로 관리했다.
			m_Range
		);
	}

	DirectX::XMMATRIX Light::GetShadowMatrix()
	{
		// rasterTek에서는 직교투영행렬을 사용한다.
		return DirectX::XMMatrixMultiply(GetViewMatrix(), GetProjectionMatrix());
	}

	void Light::SetShadowMapSize(float size)
	{ 
		// 추후 2의 제곱으로 확인해야 할 듯
		if (m_ShadowMapSize == size)
			return;

		m_ShadowMapSize = size;

		// 갱신을 위해 필요하다. 추후 좀 다듬자.
		m_pShadowMap->CreateDepthStencilView(m_ShadowMapSize, m_ShadowMapSize, DXGI_FORMAT_R32_TYPELESS);
	}
}