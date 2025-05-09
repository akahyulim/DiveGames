#pragma once
#include "EntityManager.h"

namespace Dive
{
	class SystemBase;

	class DvWorld
	{
	public:
		DvWorld() = default;
		virtual ~DvWorld() = default;

		void Destroy();

		void Update();

		EntityManager& GetEntityManager() { return m_EntityManager; }

		template<typename T>
		T* GetSystem() 
		{
			for (auto& system : m_Systems) 
			{
				if (auto castedSystem = dynamic_cast<T*>(system)) 
					return castedSystem;
			}

			auto newSystem = new T(this);
			m_Systems.emplace_back(newSystem);
			return newSystem;
		}

	private:
	private:
		EntityManager m_EntityManager;
		std::vector<SystemBase*> m_Systems;
	};
}