#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	
	class Renderable : public Component
	{
	public:
		COMPONENT_CLASS_TYPE(Unknown)

		Renderable(GameObject* pGameObject) :Component(pGameObject) {}
		Renderable(GameObject* pGameObject, unsigned long long id) :Component(pGameObject, id) {}
		virtual ~Renderable() = default;

		// Behaviour�� Enalbed���� �ٸ���. �̰� ���̴��� �������̴�.
		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

		bool IsVisible() const { return m_bVisible; }

	protected:
	private:
		// ����
		// count
		// stride

		// ����Ƽ�� ��Ұ� �� ����.
		bool m_bEnabled = true;
		bool m_bVisible = true;	// �̰� �ø� ������ �����ϴµ� ����ϴ� �� ����.
	};
}