#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	// �̸��� ���ϼ��� ������.
	// ������� �ٲ�� �ұ�...
	class RendererComponent : public Component
	{
	public:
		COMPONENT_CLASS_TYPE(Unknown)

		RendererComponent(GameObject* pGameObject) : Component(pGameObject) {}
		virtual ~RendererComponent() = default;

		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

		bool IsVisible() const { return m_bVisible; }

	protected:
	protected:
		// ����Ƽ�� ��Ұ� �� ����.
		bool m_bEnabled = true;
		bool m_bVisible = true;	// �̰� �ø� ������ �����ϴµ� ����ϴ� �� ����.
	};
}