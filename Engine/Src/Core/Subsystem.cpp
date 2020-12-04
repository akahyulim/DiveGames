#include "DivePch.h"
#include "Subsystem.h"
#include "Context.h"


namespace Dive
{
	void Subsystem::SubscribeEvent(size_t eventType, EventHandler * handler)
	{
		if (!handler)
			return;

		// handler ����
		handler->SetSenderAndEventType(nullptr, eventType);

		// ���� �̺�Ʈ�� ó���ϴ� handler�� Ȯ��
		// �̹� �����Ѵٸ� �����Ѵٴ� �ǹ̿��� ���� �� ���� ���� ����
		m_eventHandlers.push_front(handler);

		m_context->AddEventReceiver(this, eventType);
	}

	void Subsystem::UnsubscribeEvent(size_t eventType)
	{
		// m_eventHandler���� ã�´�.
		// �����Ѵٸ� �����, context������ ����
		m_context->RemoveEventReceiver(eventType);
	}

	void Subsystem::SendEvent(size_t eventType)
	{
		// context�κ��� �ڽ��� �߽��ϴ� event�� ������ group�� ȹ��
		auto group = m_context->GetReceivers(eventType);
		if (!group)
			return;

		// �ش� �������� onEvent�� ȣ��
		for (auto& receiver : group->m_receivers)
		{
			receiver->onEvent(nullptr, eventType);
		}
	}

	void Subsystem::onEvent(Subsystem * sender, size_t eventType)
	{
		if (m_eventHandlers.empty())
			return;

		// �ڽ��� ������ eventType�� �´� handler ȹ��
		for (auto& handler : m_eventHandlers)
		{
			if (handler->GetEventType() == eventType)
			{
				// handler�� invoke ȣ��
				handler->Invoke();
				return;
			}
		}
	}
}