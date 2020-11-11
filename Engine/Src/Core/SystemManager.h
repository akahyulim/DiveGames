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
		SystemManager(Engine* engine) : m_Engine(engine) {}
		~SystemManager()
		{
			for (auto& system : m_Systems)
			{
				//system.reset();
				delete system;
				system = nullptr;
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

			//m_Systems.emplace_back(std::make_shared<T>(this));
			m_Systems.emplace_back(static_cast<ISystem*>(new T(this)));
		}

		template<typename T>
		T* GetSystem() const
		{
			ValidateSystemType<T>();

			for (const auto& system : m_Systems)
			{
				if (typeid(T) == typeid(*system))
					return dynamic_cast<T*>(system);
					//return static_cast<T*>(system.get());
			}
			return nullptr;
		}

		Engine* GetEngine() const
		{
			return m_Engine;
		}

	private:
		Engine* m_Engine = nullptr;
		//std::vector<std::shared_ptr<ISystem>> m_Systems;
		std::vector<ISystem*> m_Systems;
	};
}
