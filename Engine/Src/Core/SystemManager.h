#pragma once
#include "DivePch.h"
#include "System.h"
#include "Log.h"
#include "DiveDefs.h"

namespace Dive
{
	class Engine;

	// 여전히 Engine을 raw pointer로 가진다.
	class SystemManager : public std::enable_shared_from_this<SystemManager>
	{
	public:
		SystemManager(Engine* engine) : m_Engine(engine) {}

		~SystemManager()
		{
			for (auto& system : m_Systems)
			{
				system.reset();
			}
			m_Systems.clear();

			CORE_TRACE("Call SystemManager's Destructor ======================");
			CORE_TRACE("Alive system's count: {:d}", m_Systems.size());
		}

		bool Initialize()
		{
			for (const auto& system : m_Systems)
			{
				if (!system->Initialize())
				{
					CORE_ERROR("", typeid(*system).name());
					return false;
				}
			}
			return true;
		}

		void Update()
		{
			for (const auto& system : m_Systems)
				system->Update();
		}

		template<typename T>
		void RegisterSystem()
		{
			ValidateSystemType<T>();

			m_Systems.emplace_back(std::make_shared<T>(shared_from_this()));
		}

		template<typename T>
		T* GetSystem() const
		{
			ValidateSystemType<T>();

			for (const auto& system : m_Systems)
			{
				if (typeid(T) == typeid(*system))
					return static_cast<T*>(system.get());
			}
			return nullptr;
		}

		Engine* GetEngine() const
		{
			return m_Engine;
		}

	private:
		Engine* m_Engine = nullptr;
		std::vector<std::shared_ptr<System>> m_Systems;
	};
}
