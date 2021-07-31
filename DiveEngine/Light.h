#pragma once
#include "Component.h"
#include "GraphicsInclude.h"

namespace dive
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

	// �ϴ� Directional����...
	class Light : public Component
	{
	public:
		Light(GameObject* pGameObject);
		~Light();

		// �������̵� �Լ�
		void Update(float deltaTime) override;
		void Serialize(FileStream* pFileStream) override;
		void Deserialize(FileStream* pFileStream) override;

		// ��ǻ� �Լ��� ���� ����.
		// ��κ� Get / Set�̴�.

		eLightType GetType() const { return m_Type; }
		void SetType(eLightType type) { m_Type = type; }

		DirectX::XMFLOAT4 GetColor() const { return m_Color; }
		void SetColor(const DirectX::XMFLOAT4& color) { m_Color = color; }

		float GetIntensity() const { return m_Intensity; }
		void SetIntensity(float value) { m_Intensity = value; }

		float GetRange() const { return m_Range; }
		void SetRange(float range) { m_Range = range; }

	private:

	private:
		eLightType m_Type;
		DirectX::XMFLOAT4 m_Color;
		float m_Intensity;
		float m_Range;
	};
}