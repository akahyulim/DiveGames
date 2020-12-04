#pragma once


namespace Dive
{
	class Subsystem;
	class EventReceiverGroup;

	class Context
	{
	public:
		Context() = default;
		~Context();

		template<typename T>
		void RegisterSubsystem();

		template<typename T>
		void RemoveSubsystem();

		template<typename T>
		std::shared_ptr<T> GetSubsystem();

		void AddEventReceiver(Subsystem* system, size_t eventType);
		void RemoveEventReceiver(size_t eventType);

		// 이것도 shared_ptr 혹은 weak_ptr로 전달해야 한다.
		// 그런데 사용하는 측은 그냥 포인터로 보내기도 하더라...
		EventReceiverGroup* GetReceivers(size_t eventType);

	private:
		std::vector<std::shared_ptr<Subsystem>> m_subsystems;
		// shared_ptr로 변경하자.
		std::unordered_map<size_t, EventReceiverGroup*> m_eventReceivers;
	};

	template<typename T>
	void Context::RegisterSubsystem()
	{
		m_subsystems.emplace_back(std::make_shared<T>(this));
	}

	template<typename T>
	void Context::RemoveSubsystem()
	{
	}

	template<typename T>
	std::shared_ptr<T> Context::GetSubsystem()
	{
		for (auto& subsystem : m_subsystems)
		{
			if (typeid(T) == typeid(*subsystem.get()))
				return std::static_pointer_cast<T>(subsystem);
		}

		return nullptr;
	}

	class EventReceiverGroup
	{
	public:

		void Add(Subsystem* system);
		void Remove(Subsystem* system);

		std::vector<Subsystem*> m_receivers;

	private:
	};
}