#pragma once


namespace Dive
{
	class Dive_Object;
	class Dive_EventReceiverGroup;

	class Dive_Context
	{
	public:
		Dive_Context() = default;
		~Dive_Context();

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

		void AddEventReceiver(size_t eventType, Dive_Object* subsystem);
		void RemoveEventReceiver(size_t eventType, Dive_Object* subsystem);

		Dive_EventReceiverGroup* GetEventReceiverGroup(size_t eventType);

	private:
		std::vector<Dive_Object*> m_subsystems;
		std::unordered_map<size_t, Dive_EventReceiverGroup*> m_eventReceivers;
	};

	class Dive_EventReceiverGroup
	{
	public:
		Dive_EventReceiverGroup()	= default;
		~Dive_EventReceiverGroup()	= default;

		void Add(Dive_Object* subsystem);
		void Remove(Dive_Object* subsystem);
		bool IsExist(Dive_Object* subsystem);

	public:
		std::vector<Dive_Object*> m_receivers;
	};
}