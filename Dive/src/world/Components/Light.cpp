#include "stdafx.h"
#include "Light.h"
#include "Transform.h"
#include "graphics/Graphics.h"
#include "graphics/ConstantBuffer.h"

namespace Dive
{
	Light::Light(GameObject* gameObject)
		: Component(gameObject)
		, m_lightType(eLightType::Directional)
		, m_color(1.0f, 1.0f, 1.0f)
		, m_range(50.0f)
		, m_innerAngle(DirectX::XMConvertToRadians(15.0f))
		, m_outerAngle(DirectX::XMConvertToRadians(30.0f))
	{
	}

	DirectX::XMFLOAT3 Light::GetPosition() const
	{
		assert(GetTransform());
		return GetTransform()->GetPosition();
	}
	
	DirectX::XMFLOAT3 Light::GetDirection() const
	{
		assert(GetTransform());
		return GetTransform()->GetForward();
	}
}