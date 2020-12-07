#include "DivePch.h"
#include "Dive_Object.h"
#include "Dive_Context.h"
#include "DiveDefs.h"


namespace Dive
{
	Dive_Object::Dive_Object(Dive_Context * context)
		: m_context(context)
	{
		assert(m_context);
	}

	Dive_Object::~Dive_Object()
	{
		// context���� event sender ����� ����
	}

	void Dive_Object::SubscribeEvent(size_t eventType, Dive_EventHandler * handler)
	{
		if (!handler)
			return;

		handler->SetHandler(nullptr, eventType);

		// ���� Ÿ�� �ڵ鷯 Ȯ��
		for (const auto& existHandle : m_eventHandlers)
		{
			if (existHandle->GetEventType() == handler->GetEventType())
				return;
		}

		m_eventHandlers.push_back(handler);

		m_context->AddEventReceiver(eventType, this);
	}
	
	void Dive_Object::UnsubscibeEvent(size_t eventType)
	{
		auto it = m_eventHandlers.begin();
		for (it; it != m_eventHandlers.end();)
		{
			if ((*it)->GetEventType() == eventType)
			{
				SAFE_DELETE(*it);
				it = m_eventHandlers.erase(it);
				break;
			}
			else
				++it;
		}

		m_context->RemoveEventReceiver(eventType, this);
	}
	
	// �Ű������� SendEvent�� �����Ѵ�.
	// �� �Լ��� ��쿣 �� VariantMap�� ������ �� OnEvent�� �����Ѵ�.
	// ��, onEvent, Invoke ��� VariantMap�� �޾ƾ� �Ѵ�.
	void Dive_Object::SendEvent(size_t eventType)
	{
		const auto* group = m_context->GetEventReceiverGroup(eventType);
		if (!group)
			return;

		for (auto receiver : group->m_receivers)
		{
			// ������ ��������� sender�� nullptr�� �ߴ�.
			receiver->onEvent(this, eventType);
		}
	}
	
	void Dive_Object::onEvent(Dive_Object * sender, size_t eventType)
	{
		if (m_eventHandlers.empty())
			return;

		for (auto handler : m_eventHandlers)
		{
			if (handler->GetEventType() == eventType)
			{
				handler->Invoke();
				return;
			}
		}
	}
}