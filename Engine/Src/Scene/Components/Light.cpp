#include "DivePch.h"
#include "Light.h"
#include "Transform.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	Light::Light(GameObject* pGameObject)
		: Component(pGameObject)
		, m_Type(eLightType::Directional)
		, m_LookAt(0.0f, 0.0f, 0.0f)
		, m_Color(1.0f, 1.0f, 1.0f)
		, m_Range(10.0f)
		, m_OuterAngle(0.0f)
		, m_InnerAngle(0.0f)
		, m_bEnabled(true)
	{
	}
	
	DirectX::XMFLOAT3 Light::GetDirection()
	{
		DirectX::XMVECTOR vecDir;
		DirectX::XMFLOAT3 dir = { 0.0f, 0.0f, 0.0f };

		switch (m_Type)
		{
		case eLightType::Directional:
		{
			vecDir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&m_LookAt));
			DirectX::XMStoreFloat3(&dir, vecDir);
			break;
		}
		case eLightType::Spot:
		{
			vecDir = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_LookAt), GetTransform()->GetPositionVector());
			vecDir = DirectX::XMVector3Normalize(vecDir);
			DirectX::XMStoreFloat3(&dir, vecDir);
			break;
		}
		default:
			break;
		}

		return dir;
	}
}