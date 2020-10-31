#pragma once
#include "DivePch.h"
#include "ISystem.h"
#include "Log.h"
#include "DiveDefs.h"

namespace Dive
{
	class Engine;

	class SystemManager
	{
	public:
		SystemManager(Engine* pEngine) { m_pEngine = pEngine; }
		~SystemManager()
		{
			for (auto& system : m_Systems)
			{
				system.reset();
			}
			m_Systems.clear();
		}

		bool Initialize()
		{
			for (const auto& system : m_Systems)
			{
				if (!system->Initialize())
				{
					CORE_ERROR("SystemManager::Initialize>> Failed to initialize {:s} object.", typeid(*system).name());
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

			m_Systems.emplace_back(std::make_shared<T>(this));
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
			return m_pEngine;
		}

	private:
		Engine* m_pEngine = nullptr;
		std::vector<std::shared_ptr<ISystem>> m_Systems;
	};
}
