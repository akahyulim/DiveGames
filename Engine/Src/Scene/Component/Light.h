#pragma once
#include "Component.h"
#include "../../Renderer/Graphics/GraphicsInclude.h"

namespace Dive
{
	class GameObject;
	class FileStream;

	enum class eLightType
	{
		Directional,
		Spot,
		Point,
		//Area,
	};

	// 일단 Directional부터...
	class Light : public Component
	{
	public:
		Light(GameObject* pGameObject);
		~Light();

		// 오버라이드 함수
		void Update(float deltaTime) override;
		void Serialize(FileStream* pFileStream) override;
		void Deserialize(FileStream* pFileStream) override;

		// 사실상 함수가 거의 없다.
		// 대부분 Get / Set이다.

		// Component의 Type과 이름이 겹친다.
		// 그런데... 일관성을 위해서 여긴 그대로 두는게 나을 것 같은데...
		eLightType GetLightType() const { return m_LightType; }
		void SetLightType(eLightType type) { m_LightType = type; }

		DirectX::XMFLOAT4 GetColor() const { return m_Color; }
		void SetColor(const DirectX::XMFLOAT4& color) { m_Color = color; }

		float GetIntensity() const { return m_Intensity; }
		void SetIntensity(float value) { m_Intensity = value; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

	private:

	private:
		eLightType m_LightType;
		DirectX::XMFLOAT4 m_Color;
		float m_Intensity;
		float m_Range;
	};
}