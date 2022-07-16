#include "divepch.h"
#include "DvContext.h"
#include "DvObject.h"

namespace Dive
{
	void EventReceiverGroup::Add(DvObject* pReceiver)
	{
		if (!pReceiver)
			return;

		m_Receivers.emplace_back(pReceiver);
	}

	void EventReceiverGroup::Remove(DvObject* pReceiver)
	{
		auto it = std::find(m_Receivers.begin(), m_Receivers.end(), pReceiver);
		if (it != m_Receivers.end())
			m_Receivers.erase(it);
	}

	DvContext::DvContext()
	{
	}
	
	DvContext::~DvContext()
	{
		// remove subsystems: audio, ui, input, renderer, graphics
		// 윈도우 때문에 graphics가 가장 마지막인듯
		// 전부 다 가 아니다...

		m_Subsystems.clear();

	}

	void DvContext::RegisterSubsystem(DvObject* pObject)
	{
		if (!pObject)
			return;

		m_Subsystems[pObject->GetType()] = pObject;
	}

	void DvContext::RemoveSubsystem(StringHash type)
	{
		auto it = m_Subsystems.find(type);
		if (it != m_Subsystems.end())
			m_Subsystems.erase(it);
	}

	DvObject* DvContext::GetSubsystem(StringHash type) const
	{
		auto it = m_Subsystems.find(type);
		if (it != m_Subsystems.end())
			return it->second;
	
		return nullptr;
	}

	void DvContext::AddEventReceiver(StringHash eventType, DvObject* pReceiver)
	{
		if (!pReceiver)
			return;

		auto pGroup = m_EventReceivers[eventType];
		if (!pGroup)
		{
			pGroup = new EventReceiverGroup;
			m_EventReceivers[eventType] = pGroup;
		}

		pGroup->Add(pReceiver);
	}

	void DvContext::RemoveEventReceiver(StringHash eventType, DvObject* pReceiver)
	{
		if (!pReceiver)
			return;

		auto pGroup = m_EventReceivers[eventType];
		if (!pGroup)
			return;

		pGroup->Remove(pReceiver);
	}

	EventReceiverGroup* DvContext::GetEventReceivers(StringHash eventType)
	{
		return m_EventReceivers[eventType];
	}
}