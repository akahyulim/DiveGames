#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class GameObject;

	// �������� Component�� ����� Behavior�� ����ߴ�.
	// Behavior�� enable ���θ� �Ǵ��ϴ� Component ��� Ŭ�����̴�.
	class Renderable : public Component
	{
	public:
		explicit Renderable(Context* context, GameObject* owner);
		~Renderable();

	private:
	private:
		// Rendering�� �ʿ��� Resouce���� �������� ����.
	};
}