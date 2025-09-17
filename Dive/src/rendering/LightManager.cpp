#include "stdafx.h"
#include "LightManager.h"
#include "graphics/Graphics.h"
#include "graphics/ConstantBuffer.h"
#include "world/World.h"
#include "world/components/Light.h"
#include "world/components/Transform.h"

namespace Dive
{
	const size_t MAX_LIGHTS = 16;

	struct alignas(16) Lights
	{
		DirectX::XMFLOAT4 ambientColor;
		int lightCount = 0;
		DirectX::XMFLOAT3 padding{};
		LightData lights[MAX_LIGHTS]{};
	};

	LightManager::LightManager()
	{
		m_cbLights = std::make_unique<ConstantBuffer>(static_cast<uint32_t>(sizeof(Lights)));
		if(!m_cbLights) DV_LOG(LightManager, err, "[::LightManager] Lights Buffer 생성 실패");
	}

	// Base와 Add 선별 및 정렬
	void LightManager::Update(World* world)
	{
		assert(world);

		m_lights.clear();
		m_lights = world->GetLights();

		std::sort(m_lights.begin(), m_lights.end(), [](Light* a, Light* b) {
			return a->GetLightType() < a->GetLightType();
			});
	}

	void LightManager::Bind()
	{
		Lights data{};
		data.ambientColor = m_ambientColor;
		data.lightCount = static_cast<int>(m_lights.size());
		for (size_t i = 0; i != m_lights.size(); i++)
		{
			data.lights[i].type = static_cast<uint32_t>(m_lights[i]->GetLightType());
			data.lights[i].color = m_lights[i]->GetColor();
			data.lights[i].position = m_lights[i]->GetPosition();
			data.lights[i].intensity = m_lights[i]->GetIntensity();
			data.lights[i].rangeRcp = m_lights[i]->GetRangeRcp();
			data.lights[i].direction = m_lights[i]->GetDirection();
			data.lights[i].innerAngle = m_lights[i]->GetInnerAngleRadians();
			data.lights[i].outerAngle = m_lights[i]->GetOuterAngleRadians();
		}
		
		m_cbLights->Update<Lights>(data);
		m_cbLights->BindPS(eCBufferSlotPS::Light);
	}
}