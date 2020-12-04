#include "DivePch.h"
#include "Context.h"
#include "Subsystem.h"


namespace Dive
{
	Context::~Context()
	{
		for (size_t i = 0; i != m_subsystems.size(); i++)
			m_subsystems[i].reset();
		m_subsystems.clear();

		// m_eventReceiver�� EventReceiverGroup �ʱ�ȭ�� ��Ű�� �ʾҴ�.
	}

	void Context::AddEventReceiver(Subsystem * system, size_t eventType)
	{
		auto* group = m_eventReceivers[eventType];
		if (!group)
			group = new EventReceiverGroup;
		group->Add(system);
		m_eventReceivers[eventType] = group;
	}

	void Context::RemoveEventReceiver(size_t eventType)
	{
	}

	EventReceiverGroup * Context::GetReceivers(size_t eventType)
	{
		auto it = m_eventReceivers.find(eventType);

		return it != m_eventReceivers.end() ? it->second : nullptr;
	}

	void EventReceiverGroup::Add(Subsystem * system)
	{
		if (system)
			m_receivers.push_back(system);
	}

	void EventReceiverGroup::Remove(Subsystem * system)
	{
		// ã�� �� ����
	}
}
