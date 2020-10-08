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
		SystemManager(Engine* engine) { m_engine = engine; }
		~SystemManager()
		{
			for (auto& system : m_systems)
			{
				system.reset();
			}
			m_systems.clear();
		}

		bool Initialize()
		{
			for (const auto& system : m_systems)
			{
				if (!system->Initialize())
				{
					CORE_ERROR("{:s}의 초기화에 실패하였습니다", typeid(*system).name());
					return false;
				}
			}
			return true;
		}

		void Update()
		{
			for (const auto& system : m_systems)
				system->Update();
		}

		template<typename T>
		void RegisterSystem()
		{
			ValidateSystemType<T>();

			m_systems.emplace_back(std::make_shared<T>(this));
		}

		template<typename T>
		T* GetSystem() const
		{
			ValidateSystemType<T>();

			for (const auto& system : m_systems)
			{
				if (typeid(T) == typeid(*system))
					return static_cast<T*>(system.get());
			}
			return nullptr;
		}

		Engine* GetEngine() const
		{
			return m_engine;
		}

	private:
		Engine* m_engine = nullptr;
		std::vector<std::shared_ptr<ISystem>> m_systems;
	};
}
