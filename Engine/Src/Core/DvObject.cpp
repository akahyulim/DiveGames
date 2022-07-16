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
		// dv assert�� �ٲٱ�?
		assert(m_pContext);
	}

	void DvObject::OnEvent(DvObject* pSender, StringHash eventType, VariantMap& eventData)
	{
		// ���ù� �κ��̴�.
		// �ڽ��� �ڵ鷯 ����Ʈ���� ������ �̺�Ʈ Ÿ���� ã�� ��
		auto it = m_EventHandlers.begin();
		for (it; it != m_EventHandlers.end(); ++it)
		{
			if ((*it)->GetEventType() == eventType)
			{
				// �ش� �ڵ鷯�� Invoke�� eventData�� �����Ѵ�.
				// Invoke �Լ��� �ڵ鷯 ��ü Ŭ������ �����Ǹ�, function���� ��ϵ� �Լ��� VariantMap�� �����Ѵ�.

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

	// �ϴ� specific�� ����
	void DvObject::SubscribeEvent(DvObject* pSender, StringHash eventType, DvEventHandler* pHandler)
	{
		if (!pSender || !pHandler)
		{
			delete pHandler;
			return;
		}

		pHandler->SetSenderAndEventType(pSender, eventType);

		// �������� ���� ����
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