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
		RendererComponent(GameObject* pGameObject) : Component(pGameObject) {}
		virtual ~RendererComponent() = default;

	protected:
	protected:
		// ����Ƽ�� ��Ұ� �� ����.
	};
}