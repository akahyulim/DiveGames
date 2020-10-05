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
			for (auto& pSystem : m_systems)
			{
				SAFE_DELETE(pSystem);
			}
			m_systems.clear();
		}

		bool Initialize()
		{
			for (const auto& pSystem : m_systems)
			{
				if (!pSystem->Initialize())
				{
					CORE_ERROR("{:s}�� �ʱ�ȭ�� �����Ͽ����ϴ�", typeid(*pSystem).name());
					return false;
				}
			}
			return true;
		}

		void Update(float deltaTime = 0.0f)
		{
			for (const auto& pSystem : m_systems)
				pSystem->Update(deltaTime);
		}

		// ������ ����� ������ �ʿ䰡 �ֳ�?
		void RegisterSystem(ISystem* pSystem)
		{
			if (!pSystem)
				return;

			m_systems.push_back(pSystem);
		}

		template<typename T>
		T* GetSystem() const
		{
			for (const auto& pSystem : m_systems)
			{
				if (typeid(T) == typeid(*pSystem))
					return dynamic_cast<T*>(pSystem);
			}
			return nullptr;
		}

		Engine* GetEngine() const
		{
			return m_pEngine;
		}

	private:
		Engine* m_pEngine = nullptr;
		std::vector<ISystem*> m_systems;
	};
}
