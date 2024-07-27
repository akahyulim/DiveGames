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
		//, m_pShadowMap(nullptr)
		//, m_ShadowMapSize(0.0f)
		, m_bShadowEnabled(false)
		, m_pCBufferVS(nullptr)
		, m_pCBufferPS(nullptr)
	{
	}

	DvLight::~DvLight()
	{
		DV_DELETE(m_pCBufferPS);
		DV_DELETE(m_pCBufferVS);
	}

#pragma pack(push, 1)
	struct CB_DIRECTIONAL_VS
	{
		// shadowViewProj
	};

	struct CB_DIRECTIONAL_PS
	{
		DirectX::XMFLOAT3 color;
		float padding;

		DirectX::XMFLOAT3 direction;
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
		pMappedData->color = m_Color;
		pMappedData->direction = m_pGameObject->GetForward();
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
		DirectX::XMFLOAT3 color;
		float rangeRcp;

		DirectX::XMFLOAT3 position;
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
			pMappedData->color = m_Color;
			pMappedData->rangeRcp = 1 / m_Range;
			pMappedData->position = m_pGameObject->GetPosition();
			pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;
			m_pCBufferPS->Unmap();
		}
	}

#pragma pack(push, 1)
	struct CB_SPOTLIGHT_DS
	{
		DirectX::XMMATRIX world;

		float sinOtterAngle;
		float cosOutterAngle;
		float padding[2];
	};

	struct CB_SPOTLIGHT_PS
	{
		DirectX::XMFLOAT3 color;
		float rangeRcp;

		DirectX::XMFLOAT3 position;
		float cosOutterCone;

		DirectX::XMFLOAT3 direction;
		float cosConeAttRange;

		uint32_t shadowEnabled;
		float padding[3];
	};
#pragma pack(pop)

	SpotLight::SpotLight()
		: DvLight(nullptr, eLightType::Spot)
		, m_pCBufferDS(nullptr)
		, m_Range(0.0f)
		, m_InnerAngle(0.0f)
		, m_OutterAngle(0.0f)
	{
	}

	SpotLight::SpotLight(GameObject* pGameObject)
		: DvLight(pGameObject, eLightType::Spot)
		, m_pCBufferDS(nullptr)
		, m_Range(0.0f)
		, m_InnerAngle(0.0f)
		, m_OutterAngle(0.0f)
	{
	}

	SpotLight::~SpotLight()
	{
		DV_DELETE(m_pCBufferDS);
	}

	void SpotLight::Update()
	{
		float outterAngle = DirectX::XMConvertToRadians(m_OutterAngle);
		float innerAngle = DirectX::XMConvertToRadians(m_InnerAngle);

		float sinOutterAngle = DirectX::XMScalarSin(outterAngle);
		float cosOutterAngle = DirectX::XMScalarCos(outterAngle);
		float cosInnerAngle = DirectX::XMScalarCos(innerAngle);

		// ds constant buffer
		{
			if (!m_pCBufferDS)
				m_pCBufferDS = ConstantBuffer::Create("CB_SPOTLIGHT_DS", sizeof(CB_SPOTLIGHT_DS));

			auto scale = DirectX::XMMatrixScaling(m_Range, m_Range, m_Range);
			auto trans = m_pGameObject->GetMatrix();
			auto world = DirectX::XMMatrixTranspose(scale * trans);

			auto pMappedData = reinterpret_cast<CB_SPOTLIGHT_DS*>(m_pCBufferDS->Map());
			pMappedData->world = world;
			pMappedData->sinOtterAngle = sinOutterAngle;
			pMappedData->cosOutterAngle = cosOutterAngle;
			m_pCBufferDS->Unmap();
		}

		// ps constant buffer
		{
			if (!m_pCBufferPS)
				m_pCBufferPS = ConstantBuffer::Create("CB_SPOTLIGHT_PS", sizeof(CB_SPOTLIGHT_PS));

			auto pMappedData = reinterpret_cast<CB_SPOTLIGHT_PS*>(m_pCBufferPS->Map());
			pMappedData->color = m_Color;
			pMappedData->rangeRcp = 1.0f / m_Range;
			pMappedData->position = m_pGameObject->GetPosition();
			pMappedData->cosOutterCone = cosOutterAngle;
			pMappedData->direction = m_pGameObject->GetForward();
			pMappedData->cosConeAttRange = cosInnerAngle - cosOutterAngle;
			pMappedData->shadowEnabled = m_bShadowEnabled ? 1 : 0;
			m_pCBufferPS->Unmap();
		}
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
			m_CBufferPS.direction = m_pGameObject->GetForward();
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
			m_CBufferPS.position = m_pGameObject->GetPosition();
			m_CBufferPS.rangeRcp = 1.0f / GetRange();
			break;
		}
		case eLightType::Spot:
		{
			m_CBufferVS.shadow = DirectX::XMMatrixTranspose(GetShadowMatrix());

			m_CBufferPS.options = (1U << 2);
			m_CBufferPS.position = m_pGameObject->GetPosition();
			m_CBufferPS.direction = m_pGameObject->GetForward();
			m_CBufferPS.rangeRcp = 1.0f / GetRange();
			m_CBufferPS.outerConeAngle = GetOuterAngleRadian();
			m_CBufferPS.innerConeAngle = GetInnerAngleRadian();
			m_CBufferPS.shadow = DirectX::XMMatrixTranspose(GetShadowMatrix());
			break;
		}
		default:
			break;
		}
		m_CBufferPS.color = { GetColor().x * GetColor().x
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