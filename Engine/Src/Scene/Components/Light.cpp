#include "DivePch.h"
#include "Light.h"
#include "Core/CoreDefs.h"
#include "Graphics/RenderTexture.h"
#include "Scene/GameObject.h"

#include "Graphics/Graphics.h"

namespace Dive
{
	// ������ �𸣰����� ��ġ�� 0, 0, 0���� �����ϸ� ������ �߻��Ѵ�.
	Light::Light(GameObject* pGameObject)
		: Component(pGameObject)
		, m_Type(eLightType::Directional)
		, m_Color(1.0f, 1.0f, 1.0f)
		, m_Range(10.0f)
		, m_OuterAngle(0.0f)
		, m_InnerAngle(0.0f)
		, m_bEnabled(true)
		, m_pShadowMap(nullptr)
		, m_ShadowMapSize(1024.0f)
	{
		m_pShadowMap = new RenderTexture();
		m_pShadowMap->SetDepthStencil(m_ShadowMapSize, m_ShadowMapSize, DXGI_FORMAT_R32_TYPELESS);
		//m_pShadowMap->SetDepthStencil(1600, 900, DXGI_FORMAT_R32_TYPELESS);
		m_pShadowMap->SetAddressMode(D3D11_TEXTURE_ADDRESS_CLAMP);
		m_pShadowMap->SetFilter(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
		m_pShadowMap->SetComparisonFunc(D3D11_COMPARISON_LESS_EQUAL);
	}

	Light::~Light()
	{
		DV_DELETE(m_pShadowMap);

		DV_CORE_TRACE("������Ʈ({0:s}'s {1:s}) �Ҹ�", GetName(), GetTypeName());
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
			5.0f,			// å������ ����� ��� ������ �����ߴ�.
			m_Range
		);
	}

	DirectX::XMMATRIX Light::GetShaodwMatrix()
	{
		return DirectX::XMMatrixMultiply(GetViewMatrix(), GetProjectionMatrix());
	}

	void Light::SetShadowMapSize(float size)
	{
		// ���� 2�� �������� Ȯ���ؾ� �� ��
		if (m_ShadowMapSize == size)
			return;

		m_ShadowMapSize = size;

		// ������ ���� �ʿ��ϴ�. ���� �� �ٵ���.
		m_pShadowMap->SetDepthStencil(m_ShadowMapSize, m_ShadowMapSize, DXGI_FORMAT_R32_TYPELESS);
	}
}