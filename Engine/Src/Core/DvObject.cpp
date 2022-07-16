#include "divepch.h"
#include "DvObject.h"
#include "DvContext.h"

namespace Dive
{
	TypeInfo::TypeInfo(const char* pTypeName, const TypeInfo* pBaseTypeInfo)
		: m_Type(pTypeName),
		m_TypeName(pTypeName),
		m_pBaseTypeInfo(pBaseTypeInfo)
	{
	}

	bool TypeInfo::IsTypeOf(StringHash type) const
	{
		const TypeInfo* pCurrent = this;

		while (pCurrent)
		{
			if (pCurrent->GetType() == type)
				return true;

			pCurrent = pCurrent->GetBaseTypeInfo();
		}

		return false;
	}

	bool TypeInfo::IsTypeOf(const TypeInfo* pTypeInfo) const
	{
		if (!pTypeInfo)
			return false;

		const TypeInfo* pCurrent = this;

		while (pCurrent)
		{
			if (pCurrent == pTypeInfo || pCurrent->GetType() == pTypeInfo->GetType())
				return true;

			pCurrent = pCurrent->GetBaseTypeInfo();
		}

		return false;
	}

	DvObject::DvObject(DvContext* pContext)
		: m_pContext(pContext)
	{
		// dv assert로 바꾸기?
		assert(m_pContext);
	}

	void DvObject::OnEvent(DvObject* pSender, StringHash eventType, VariantMap& eventData)
	{
		// 리시버 부분이다.
		// 자신의 핸들러 리스트에서 동일한 이벤트 타입을 찾은 후
		auto it = m_EventHandlers.begin();
		for (it; it != m_EventHandlers.end(); ++it)
		{
			if ((*it)->GetEventType() == eventType)
			{
				// 해당 핸들러의 Invoke에 eventData를 전달한다.
				// Invoke 함수는 핸들러 구체 클래스에 구현되며, function으로 등록된 함수에 VariantMap을 전달한다.

				break;
			}
		}
	}

	bool DvObject::IsInstanceOf(StringHash type) const
	{
		return GetTypeInfo()->IsTypeOf(type);
	}

	bool DvObject::IsInstanceOf(const TypeInfo* typeInfo) const
	{
		return GetTypeInfo()->IsTypeOf(typeInfo);
	}

	void DvObject::SubscribeEvent(StringHash eventType, DvEventHandler* pHandler)
	{
		if (!pHandler)
			return;

		pHandler->SetSenderAndEventType(nullptr, eventType);

		auto it = m_EventHandlers.begin();
		for (it; it != m_EventHandlers.end(); ++it)
		{
			if ((*it)->GetEventType() == pHandler->GetEventType())
			{
				m_EventHandlers.erase(it);
				break;
			}
		}

		m_EventHandlers.emplace_back(pHandler);
		m_pContext->AddEventReceiver(eventType, this);
	}

	// 일단 specific은 제외
	void DvObject::SubscribeEvent(DvObject* pSender, StringHash eventType, DvEventHandler* pHandler)
	{
		if (!pSender || !pHandler)
		{
			delete pHandler;
			return;
		}

		pHandler->SetSenderAndEventType(pSender, eventType);

		// 나머지는 위와 동일
	}

	void DvObject::UnsubscribeEvent(StringHash eventType)
	{
		auto it = m_EventHandlers.begin();
		for (it; it != m_EventHandlers.end(); ++it)
		{
			if ((*it)->GetEventType() == eventType)
			{
				m_EventHandlers.erase(it);
				m_pContext->RemoveEventReceiver(eventType, this);

				return;
			}
		}
	}

	void DvObject::SendEvent(StringHash eventType)
	{
		VariantMap noEventData;
		SendEvent(eventType, noEventData);
	}

	void DvObject::SendEvent(StringHash eventType, VariantMap& eventData)
	{
		auto pReceivers = m_pContext->GetEventReceivers(eventType);
		if (!pReceivers)
			return;

		for (auto pReceiver : pReceivers->m_Receivers)
		{
			pReceiver->OnEvent(this, eventType, eventData);
		}
	}

	DvObject* DvObject::GetSubsystem(StringHash type) const
	{
		return m_pContext->GetSubsystem(type);
	}
}