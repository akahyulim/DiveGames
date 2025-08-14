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
		, m_options(0)
	{
		m_cbLight = std::make_unique<ConstantBuffer>(
			Graphics::GetDevice(),
			ePSConstantBufferSlot::Light,
			static_cast<uint32_t>(sizeof(LightData)));
		if (!m_cbLight) DV_LOG(Light, err, "[::Light] Light Buffer 생성 실패");
	}

	void Light::Bind(ID3D11DeviceContext* deviceContext)
	{
		LightData cpuBuffer{};
		cpuBuffer.color = m_color;
		cpuBuffer.position = GetTransform()->GetPosition();
		cpuBuffer.rangeRcp = 1.0f / m_range;
		cpuBuffer.direction = GetTransform()->GetForward();
		cpuBuffer.options = static_cast<uint32_t>(m_lightType);
		m_cbLight->Update<LightData>(deviceContext, cpuBuffer);
		m_cbLight->Bind(deviceContext);
	}
}