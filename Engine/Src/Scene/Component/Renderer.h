#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	// 이름의 통일성이 깨졌다.
	// 어느쪽을 바꿔야 할까...
	class RendererComponent : public Component
	{
	public:
		RendererComponent(GameObject* pGameObject) : Component(pGameObject) {}
		virtual ~RendererComponent() = default;

	protected:
	protected:
		// 유니티엔 요소가 좀 많다.
	};
}