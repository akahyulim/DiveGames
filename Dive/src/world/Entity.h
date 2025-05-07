#pragma once
#include "core/UUID.h"
#include "World.h"
#include "Components.h"
#include <entt/entt.hpp>

namespace Dive
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, World* world);
		Entity(const Entity&) = default;

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_World == other.m_World;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator UINT32() const { return static_cast<UINT32>(m_EntityHandle); }

		template<typename T, typename... Args>
		T& AddComponent(Args&... args)
		{
			DV_ASSERT(Entity, !HasComponent<T>());
			T& component = m_World->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		
			return component;
		}

		template<typename T>
		void RemoveComponent()
		{
			DV_ASSERT(Entity, HasComponent<T>());
			m_World->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		T& GetComponent()
		{
			DV_ASSERT(Entity, HasComponent<T>());
			return m_World->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			auto view = m_World->m_Registry.view<T>();
			return view.contains(m_EntityHandle);
		}

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<NameComponent>().Name; }
		eTag GetTag() { return GetComponent<TagComponent>().Tag; }
		
		World* GetWorld() const { return m_World; }

	private:
		entt::entity m_EntityHandle = entt::null;
		World* m_World = nullptr;
	};
}