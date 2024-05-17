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
		m_pShadowMap->SetDepthStencil(m_ShadowMapSize, m_ShadowMapSize, DXGI_FORMAT_R32_TYPELESS);
		m_pShadowMap->SetAddressMode(D3D11_TEXTURE_ADDRESS_CLAMP);
		m_pShadowMap->SetFilter(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
		m_pShadowMap->SetComparisonFunc(D3D11_COMPARISON_LESS_EQUAL);

		ZeroMemory(&m_CBufferVS, sizeof(VSConstBuf_Light));
		ZeroMemory(&m_CBufferPS, sizeof(PSConstBuf_Light));
	}

	Light::~Light()
	{
		DV_DELETE(m_pShadowMap);

		DV_ENGINE_TRACE("컴포넌트({0:s}'s {1:s}) 소멸", GetName(), GetTypeName());
	}

	void Light::Update()
	{
		switch (GetType())
		{
		case eLightType::Directional:
			m_CBufferPS.options = (1U << 0);
			m_CBufferPS.direction = m_pGameObject->GetForward();
			break;
		case eLightType::Point:
			m_CBufferPS.options = (1U << 1);
			m_CBufferPS.position = m_pGameObject->GetPosition();
			m_CBufferPS.rangeRcp = 1.0f / GetRange();
			break;
		case eLightType::Spot:
			m_CBufferVS.shadow = DirectX::XMMatrixTranspose(GetShaodwMatrix());

			m_CBufferPS.options = (1U << 2);
			m_CBufferPS.position = m_pGameObject->GetPosition();
			m_CBufferPS.direction = m_pGameObject->GetForward();
			m_CBufferPS.rangeRcp = 1.0f / GetRange();
			m_CBufferPS.outerConeAngle = GetOuterAngleRadian();
			m_CBufferPS.innerConeAngle = GetInnerAngleRadian();
			m_CBufferPS.shadow = DirectX::XMMatrixTranspose(GetShaodwMatrix());
			m_CBufferPS.shadowEnabled = m_bShadowEnabled ? 1: 0;	// 현재 그림자는 spot만 존재하기 때문
			break;
		default:
			break;
		}
		m_CBufferPS.color = { GetColor().x * GetColor().x
		, GetColor().y * GetColor().y
		, GetColor().z * GetColor().z };
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

	DirectX::XMMATRIX Light::GetShaodwMatrix()
	{
		return DirectX::XMMatrixMultiply(GetViewMatrix(), GetProjectionMatrix());
	}

	void Light::SetShadowMapSize(float size)
	{
		// 추후 2의 제곱으로 확인해야 할 듯
		if (m_ShadowMapSize == size)
			return;

		m_ShadowMapSize = size;

		// 갱신을 위해 필요하다. 추후 좀 다듬자.
		m_pShadowMap->SetDepthStencil(m_ShadowMapSize, m_ShadowMapSize, DXGI_FORMAT_R32_TYPELESS);
	}
}