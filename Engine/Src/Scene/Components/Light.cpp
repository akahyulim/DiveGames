#include "DivePch.h"
#include "Light.h"
#include "Core/CoreDefs.h"
#include "Graphics/RenderTexture.h"
#include "Scene/GameObject.h"

#include "Graphics/Graphics.h"

namespace Dive
{
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
		
		ZeroMemory(&m_CBufferVS, sizeof(VSConstBuf_Light));
		ZeroMemory(&m_CBufferDS, sizeof(DSConstBuf_Light));
		ZeroMemory(&m_CBufferPS, sizeof(PSConstBuf_Light));
	}

	Light::~Light()
	{
		DV_DELETE(m_pShadowMap);

		DV_ENGINE_TRACE("컴포넌트({0:s}'s {1:s}) 소멸", GetName(), GetTypeName());
	}

	void Light::Update()
	{
		ZeroMemory(&m_CBufferVS, sizeof(VSConstBuf_Light));
		ZeroMemory(&m_CBufferDS, sizeof(DSConstBuf_Light));
		ZeroMemory(&m_CBufferPS, sizeof(PSConstBuf_Light));

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