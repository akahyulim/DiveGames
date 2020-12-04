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

		// �̰͵� shared_ptr Ȥ�� weak_ptr�� �����ؾ� �Ѵ�.
		// �׷��� ����ϴ� ���� �׳� �����ͷ� �����⵵ �ϴ���...
		EventReceiverGroup* GetReceivers(size_t eventType);

	private:
		std::vector<std::shared_ptr<Subsystem>> m_subsystems;
		// shared_ptr�� ��������.
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