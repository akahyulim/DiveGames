#pragma once

namespace Dive
{
	class Object;

	class Context
	{
	public:
		Context() = default;
		~Context();

		void Clear();

		template<class T> void RegisterSubsystem()
		{
			if (IsExist<T>())
				return;

			m_subsystems.emplace_back(new T(this));
		}

		template<class T> void RemoveSubsystem()
		{
			auto it = m_subsysems.begin();
			for (it; it != m_subsystems.end();)
			{
				if ((*it)->GetTypeHash() == typeid(T).hash_code())
					it = m_subsystems.erase(it);
				else
					it++;
			}
		}

		template<class T> T* GetSubsystem()
		{
			for (const auto& subsystem : m_subsystems)
			{
				if (subsystem->GetTypeHash() == typeid(T).hash_code())
					return static_cast<T*>(subsystem);
			}

			return nullptr;
		}

		template<class T> bool IsExist()
		{
			for (const auto& subsystem : m_subsystems)
			{
				if (subsystem->GetTypeHash() == typeid(T).hash_code())
					return true;
			}

			return false;
		}

	private:
		std::vector<Object*> m_subsystems;
	};
}