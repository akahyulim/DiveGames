#pragma once
#include "EntityManager.h"
#include "systems/TransformSystem.h"

namespace Dive
{
	class World
	{
	public:
		World(const std::string& name);
		virtual ~World();

		void Clear();

		void Update();

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		EntityManager& GetEntityManager() { return m_EntityManager; }

		template<typename T>
		T* GetOrAddSystem() 
		{
			for (auto system : m_Systems) 
			{
				if (auto castedSystem = dynamic_cast<T*>(system)) 
					return castedSystem;
			}

			T* newSystem = new T(this);
			m_Systems.emplace_back(newSystem);
			return newSystem;
		}

	private:
		std::string m_Name{};
		EntityManager m_EntityManager;
		std::vector<SystemBase*> m_Systems;
	};
}