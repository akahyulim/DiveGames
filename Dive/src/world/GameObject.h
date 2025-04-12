#pragma once
#include "core/UUID.h"
#include "World.h"
#include "Components.h"
#include <entt/entt.hpp>

namespace Dive
{
	class GameObject
	{
	public:
		GameObject() = default;
		GameObject(entt::entity handle, World* world);
		GameObject(const GameObject&) = default;

		bool operator==(const GameObject& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_World == other.m_World;
		}

		bool operator!=(const GameObject& other) const
		{
			return !(*this == other);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator UINT32() const { return static_cast<UINT32>(m_EntityHandle); }

		template<typename T, typename... Args>
		T& AddComponent(Args&... args)
		{
			// assert
			T& component = m_World->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			// OnComponentAdded???
			return component;
		}

		template<typename T>
		void RemoveComponent()
		{
			// assert
			m_World->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent()
		{
			// assert
			return m_World->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			auto view = m_World->m_Registry.view<T>();
			return view.contains(m_EntityHandle);
		}

		const UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }
		
	private:
		entt::entity m_EntityHandle = entt::null;
		World* m_World = nullptr;
	};
}