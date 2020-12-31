#pragma once
#include "DivePch.h"
#include "DiveDefs.h"

#define DIVE_FIRE_EVENT(evnt)											Dive::EventDispatcher::GetInstance().Fire(evnt)

#define DIVE_SUBSCRIBE_TO_EVENT(receiver, memberFunction)				Dive::EventDispatcher::GetInstance().SubscribeToEvent(receiver, memberFunction)
#define DIVE_UNSUBSCRIBE_FROM_EVENT(receiverType, eventType, receiver)	Dive::EventDispatcher::GetInstance().UnsubscribeFromEvent<receiverType, eventType>(receiver)

// 디버그 모드로 종료시 list의 메모리 오류가 발생한다.
namespace Dive
{
	class IEvent
	{
	public:
		virtual ~IEvent() = default;
	};


	class BaseHandler
	{
	public:
		virtual ~BaseHandler() = default;

		void Call(const IEvent* evnt)
		{
			if (!evnt)
				return;

			invoke(evnt);
		}

		virtual size_t GetReceiverType()		const = 0;
		virtual std::string GetReceiverName()	const = 0;

	protected:
		virtual void invoke(const IEvent* evnt) = 0;
	};


	template<class T, typename EventType>
	class MemberFunctionHandler : public BaseHandler
	{
		typedef void(T::*MemberFunction)(EventType*);

	public:
		MemberFunctionHandler(T* receiver, MemberFunction function)
			: m_receiver(receiver), m_function(function)
		{
			assert(m_receiver);
			assert(m_function);
		}
		~MemberFunctionHandler() = default;

		size_t GetReceiverType() const { return typeid(T).hash_code(); }
		std::string GetReceiverName() const { return typeid(T).name(); }

	private:
		void invoke(const IEvent* evnt) override
		{
			(m_receiver->*m_function)(static_cast<EventType*>(evnt));
		}

	private:
		T* m_receiver;
		MemberFunction m_function;
	};


	class EventDispatcher
	{
		using ReceiverGroup = std::list<BaseHandler*>;

	public:
		static EventDispatcher& GetInstance()
		{
			static EventDispatcher instance;
			return instance;
		}

		template<class T, typename EventType>
		void SubscribeToEvent(T* receiver, void(T::*MemberFunction)(EventType*))
		{
			if (!receiver)
				return;

			size_t typeHash = typeid(EventType).hash_code();
			auto group = m_eventReceivers[typeHash];
			if (group == nullptr)
			{
				group = new ReceiverGroup;
				m_eventReceivers[typeHash] = group;
			}

			group->emplace_back(new MemberFunctionHandler<T, EventType>(receiver, MemberFunction));
		}

		template<class T, typename EventType>
		void UnsubscribeFromEvent(T* receiver)
		{
			if (!receiver)
				return;

			size_t typeHash = typeid(EventType).hash_code();
			auto group = m_eventReceivers[typeHash];
			if (group == nullptr)
				return;

			size_t receiverHash = typeid(T).hash_code();
			for (auto receiver : *group)
			{
				if (receiverHash == receiver->GetReceiverType())
				{
					group->remove(receiver);
					return;
				}
			}
		}

		void Fire(const IEvent* evnt)
		{
			if (!evnt || m_eventReceivers.empty())
				return;

			size_t eventHash = typeid(*evnt).hash_code();
			auto group = m_eventReceivers[eventHash];
			if (!group || group->empty())
				return;

			for (auto receiver : *group)
			{
				receiver->Call(evnt);
			}
		}

		template<typename EventType>
		void ClearTargetEventTypeRecevier()
		{
			size_t eventHash = typeid(EventType).hash_code();
			auto group = m_eventReceivers[eventHash];
			if (!group)
				return;

			for (auto receiver : *group)
			{
				SAFE_DELETE(receiver);
			}
			SAFE_DELETE(group);

			m_eventReceivers.erase(eventHash);
		}

		void ClearAllReceivers()
		{
			auto it = m_eventReceivers.begin();
			for (it; it != m_eventReceivers.end(); it++)
			{
				auto group = it->second;
				for (auto receiver : *group)
				{
					SAFE_DELETE(receiver);
				}
				SAFE_DELETE(group);
			}

			m_eventReceivers.clear();
		}

	private:
		EventDispatcher()	= default;
		~EventDispatcher() { ClearAllReceivers(); }

	private:
		std::unordered_map<size_t, ReceiverGroup*> m_eventReceivers;
	};
}

