#pragma once
#include "Component.h"

namespace Dive
{
    class ConstantBuffer;

    enum class eLightType
    {
        Directional,
        Point,
        Spot
    };

    struct alignas(16) LightData
    {
        DirectX::XMFLOAT3 color;
        float outerConeAngle;

        DirectX::XMFLOAT3 position;
        float rangeRcp;

        DirectX::XMFLOAT3 direction;
        float innerConeAngle;

        uint32_t options;
        int shadowEnabled;
        DirectX::XMFLOAT2 padding;

        DirectX::XMFLOAT4X4 shadow;
    };

	class Light : public Component
	{
	public:
        Light(GameObject* gameObject);
        ~Light() = default;

        static constexpr eComponentType GetType() { return eComponentType::Light; }

        void Bind(ID3D11DeviceContext* deviceContext);

        eLightType GetLightType() const { return m_lightType; }
        void SetLightType(eLightType type) { m_lightType = type; }

        DirectX::XMFLOAT3 GetColor() const { return m_color; }
        void SetColor(float r, float g, float b) { m_color = { r, g, b }; }
        void SetColor(const DirectX::XMFLOAT3& color) { m_color = color; }

    private:
	private:
        eLightType m_lightType;
        DirectX::XMFLOAT3 m_color;
        uint32_t m_options;

        std::unique_ptr<ConstantBuffer> m_cbLight;
	};
}