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

			m_subsystems.emplace_back(std::make_shared<T>(this));
		}

		template<class T> void RemoveSubsystem()
		{
			auto it = m_subsystems.begin();
			for (it; it != m_subsystems.end();)
			{
				if ((*it)->GetTypeHash() == typeid(T).hash_code())
					it = m_subsystems.erase(it);
				else
					it++;
			}
		}

		template<class T> std::shared_ptr<T> GetSubsystem()
		{
			for (auto& subsystem : m_subsystems)
			{
				if (subsystem->GetTypeHash() == typeid(T).hash_code())
					return std::static_pointer_cast<T>(subsystem);
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
		std::vector<std::shared_ptr<Object>> m_subsystems;
	};
}