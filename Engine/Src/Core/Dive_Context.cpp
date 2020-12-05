#include "DivePch.h"
#include "Dive_Context.h"
#include "Dive_Object.h"
#include "DiveDefs.h"


namespace Dive
{
	Dive_Context::~Dive_Context()
	{
		for (auto& it : m_eventReceivers)
		{
			SAFE_DELETE(it.second);
		}
		m_eventReceivers.clear();


		for (auto& subsystem : m_subsystems)
		{
			SAFE_DELETE(subsystem);
		}
		m_subsystems.clear();
	}

	void Dive_Context::AddEventReceiver(size_t eventType, Dive_Object* subsystem)
	{
		auto* group = m_eventReceivers[eventType];
		if (!group)
		{
			group = new Dive_EventReceiverGroup;
			m_eventReceivers[eventType] = group;
		}

		group->Add(subsystem);
	}

	void Dive_Context::RemoveEventReceiver(size_t eventType, Dive_Object * subsystem)
	{
		// 해당 eventType을 다루는 group에서 제거
		auto group = m_eventReceivers[eventType];
		if (group)
			group->Remove(subsystem);
	}
	
	Dive_EventReceiverGroup * Dive_Context::GetEventReceiverGroup(size_t eventType)
	{
		auto it = m_eventReceivers.find(eventType);
		return it == m_eventReceivers.end() ? nullptr : it->second;
	}

	void Dive_EventReceiverGroup::Add(Dive_Object* subsystem)
	{
		if (!subsystem || IsExist(subsystem))
			return;

		m_receivers.push_back(subsystem);
	}
	
	void Dive_EventReceiverGroup::Remove(Dive_Object* subsystem)
	{
		if (!subsystem)
			return;

		auto it = m_receivers.begin();
		for (it; it != m_receivers.end();)
		{
			if ((*it)->GetTypeHash() == subsystem->GetTypeHash())
				it = m_receivers.erase(it);
			else
				it++;
		}
	}

	bool Dive_EventReceiverGroup::IsExist(Dive_Object * subsystem)
	{
		if (!subsystem)
			return false;

		for (auto receiver : m_receivers)
		{
			if (receiver->GetTypeHash() == subsystem->GetTypeHash())
				return true;
		}

		return false;
	}
}