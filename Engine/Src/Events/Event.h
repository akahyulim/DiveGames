
// https://www.gamedev.net/tutorials/_/technical/game-programming/effective-event-handling-in-c-r2459/
// https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f
// FunctionHandler와 MethodHandler를 구분한 이유는?
// FunctionHandler::Exec()를 완벽한 전달로?
// EventDispatcher의 동적 생성 객체들을 처리해야 한다.
#pragma once
#include "DivePch.h"

namespace Dive
{
	class Event
	{
	public:
		virtual ~Event() {}
	};

	class FucntionHandler
	{
	public:
		void Exec(Event* evnt)
		{
			call(evnt);
		}

	protected:
		virtual void call(Event* evnet) = 0;
	};

	template<class T, class EventType>
	class MethodHandler : public FucntionHandler
	{
	public:
		typedef void (T::*MemberFunction)(EventType*);

		MethodHandler(T* instance, MemberFunction method)
			: m_instance(instance), m_method(method)
		{}

		void call(Event* evnt)
		{
			(m_instance->*m_method)(static_cast<EventType*>(evnt));
		}

	private:
		T* m_instance;
		MemberFunction m_method;
	};

	class EventDispatcher
	{
		using handler_list = std::list<FucntionHandler*>;

	public:
		static EventDispatcher& GetInstace()
		{
			static EventDispatcher instance;
			return instance;
		}

		template<class T, class EventType>
		void Subscribe(T* instance, void (T::*Method)(EventType*))
		{
			handler_list* handlers = m_subscribers[typeid(EventType)];

			if (handlers == nullptr)
			{
				handlers = new handler_list;
				m_subscribers[typeid(EventType)] = handlers;
			}

			handlers->push_back(new MethodHandler<T, EventType>(instance, Method));
		}

		template<typename EventType>
		void Fire(EventType* evnt)
		{
			handler_list * handlers = m_subscribers[typeid(EventType)];

			if (!handlers)
				return;

			for (auto& handler : *handlers)
			{
				if (handler != nullptr)
					handler->Exec(evnt);
			}
		}

	private:
		EventDispatcher() = default;

	private:
		std::map<std::type_index, handler_list*> m_subscribers;
	};
}