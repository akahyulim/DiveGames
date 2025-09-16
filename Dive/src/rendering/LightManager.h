#pragma once

namespace Dive
{
	class ConstantBuffer;
	class World;
	class Light;

	// 1. 매 프레임 활성화된 광원 객체 선별 저장
	// 2. 정렬
	// 3. 바인딩
	// + 개수 제한?
	class LightManager
	{ 
	public:
		LightManager();
		~LightManager() = default;

		void Update(World* world);
		void Bind();

		// 얘가 가지는 게 맞을까나...
		DirectX::XMFLOAT4 GetAmbientColor() const { return m_ambientColor; }
		void SetAmbientColor(float r, float g, float b, float a) { m_ambientColor = { r, g, b, a }; }
		void SetAmbientColor(const DirectX::XMFLOAT4& color) { m_ambientColor = color; }

	private:
	private:
		DirectX::XMFLOAT4 m_ambientColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		std::vector<Light*> m_lights;
		std::unique_ptr<ConstantBuffer> m_cbLights;
	};
}