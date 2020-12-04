#include "DivePch.h"
#include "Subsystem.h"
#include "Context.h"


namespace Dive
{
	void Subsystem::SubscribeEvent(size_t eventType, EventHandler * handler)
	{
		if (!handler)
			return;

		// handler 가공
		handler->SetSenderAndEventType(nullptr, eventType);

		// 동일 이벤트를 처리하는 handler를 확인
		// 이미 존재한다면 갱신한다는 의미에서 지운 후 받은 것을 저장
		m_eventHandlers.push_front(handler);

		m_context->AddEventReceiver(this, eventType);
	}

	void Subsystem::UnsubscribeEvent(size_t eventType)
	{
		// m_eventHandler에서 찾는다.
		// 존재한다면 지우고, context에서도 제거
		m_context->RemoveEventReceiver(eventType);
	}

	void Subsystem::SendEvent(size_t eventType)
	{
		// context로부터 자신이 발신하는 event의 구독자 group를 획득
		auto group = m_context->GetReceivers(eventType);
		if (!group)
			return;

		// 해당 구독자의 onEvent를 호출
		for (auto& receiver : group->m_receivers)
		{
			receiver->onEvent(nullptr, eventType);
		}
	}

	void Subsystem::onEvent(Subsystem * sender, size_t eventType)
	{
		if (m_eventHandlers.empty())
			return;

		// 자신이 구독한 eventType에 맞는 handler 획득
		for (auto& handler : m_eventHandlers)
		{
			if (handler->GetEventType() == eventType)
			{
				// handler의 invoke 호출
				handler->Invoke();
				return;
			}
		}
	}
}