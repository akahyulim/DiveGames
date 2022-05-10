#pragma once
#include <string>
#include <sstream>
#include "EventSystem.h"
#include "Base/Engine.h"

namespace Dive
{
	class Scene;
	class GameObject;
	class Component;

	class WindowDataEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowData)

		WindowDataEvent(const WindowData& data) : m_WindowData(data) {}

		const WindowData& GetWindowData() const { return m_WindowData; }

	private:
		WindowData m_WindowData;
	};

	class CreateSceneEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(CreateScene)

		CreateSceneEvent(Scene* pScene) : m_pCreatedScene(pScene) {}

		const Scene* GetCreatedScene() const { return m_pCreatedScene; }

	private:
		Scene* m_pCreatedScene = nullptr;
	};

	class SceneResolveEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(SceneResolve)

		SceneResolveEvent(std::vector<GameObject*>* pGameObjects) : m_pGameObjects(pGameObjects) {}

		const std::vector<GameObject*>* GetGameObjects() const { return m_pGameObjects; }

	private:
		std::vector<GameObject*>* m_pGameObjects;
	};

	class GameObjectModifyEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(GameObjectModify)

		GameObjectModifyEvent(Component* pComponent) : m_pComponent(pComponent) {}

		const Component* GetComponent() const { return m_pComponent; }

	private:
		Component* m_pComponent = nullptr;
	};
}