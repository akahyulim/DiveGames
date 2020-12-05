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
		// context에서 event sender 등록을 제거
	}

	void Dive_Object::SubscribeEvent(size_t eventType, Dive_EventHandler * handler)
	{
		if (!handler)
			return;

		handler->SetHandler(nullptr, eventType);

		// 동일 타입 핸들러 확인
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
	
	// 매개변수는 SendEvent로 전달한다.
	// 이 함수의 경우엔 빈 VariantMap을 생성한 후 OnEvent에 전달한다.
	// 즉, onEvent, Invoke 모두 VariantMap을 받아야 한다.
	void Dive_Object::SendEvent(size_t eventType)
	{
		const auto* group = m_context->GetEventReceiverGroup(eventType);
		if (!group)
			return;

		for (auto receiver : group->m_receivers)
		{
			// 이전에 만들었을땐 sender를 nullptr로 했다.
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