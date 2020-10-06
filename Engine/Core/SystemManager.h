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
				SAFE_DELETE(system);
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

		void Update(float deltaTime = 0.0f)
		{
			for (const auto& system : m_systems)
				system->Update(deltaTime);
		}

		// 생성과 등록을 따로할 필요가 있나?
		void RegisterSystem(ISystem* system)
		{
			if (!system)
				return;

			m_systems.push_back(system);
		}

		template<typename T>
		T* GetSystem() const
		{
			for (const auto& system : m_systems)
			{
				if (typeid(T) == typeid(*system))
					return dynamic_cast<T*>(system);
			}
			return nullptr;
		}

		Engine* GetEngine() const
		{
			return m_engine;
		}

	private:
		Engine* m_engine = nullptr;
		std::vector<ISystem*> m_systems;
	};
}
