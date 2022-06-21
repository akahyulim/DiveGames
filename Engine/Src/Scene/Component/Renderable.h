#pragma once
#include "Component.h"

namespace Dive
{
	class Material;
	class GameObject;
	
	class Renderable : public Component
	{
	public:
		Renderable(GameObject* pGameObject) : Component(pGameObject) {}
		virtual ~Renderable() = default;

		void CopyAttributes(Component* pCloneCom) override;

		// Behaviour�� Enalbed���� �ٸ���. �̰� ���̴��� �������̴�.
		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

		bool IsVisible() const { return m_bVisible; }

		Material* GetMaterial() { return m_pMaterial; }
		void SetMaterial(Material* pMaterial);
		void SetMaterial(const std::string& filepath);
		std::string GetMaterialName() const;
		bool HasMaterial() const { return m_pMaterial != nullptr; }
		void UseDefaultMaterial();

	protected:
	protected:
		// ����
		// count
		// stride

		// ����Ƽ�� ��Ұ� �� ����.
		bool m_bEnabled = true;
		bool m_bVisible = true;	// �̰� �ø� ������ �����ϴµ� ����ϴ� �� ����.

		Material* m_pMaterial = nullptr;
	};
}