#pragma once
#include "DivePch.h"
#include "Core/EventSystem.h"

namespace Dive
{
	class GameObject;

	// Resolve라는 이름은???
	class E_UPDATE_SCENE : public IEvent
	{
	public:
		E_UPDATE_SCENE(const std::vector<std::shared_ptr<GameObject>>& gameObjects) : m_gameObjects(gameObjects) {}

		const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const { return m_gameObjects; }

	private:
		std::vector<std::shared_ptr<GameObject>> m_gameObjects;
	};
}