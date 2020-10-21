
// https://www.gamedev.net/tutorials/_/technical/game-programming/effective-event-handling-in-c-r2459/
// https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f
// 완벽한 전달?

#pragma once
#include "DivePch.h"

namespace Dive
{
	class Event
	{
	public:
		virtual ~Event() {}
	};

	class HandlerBase
	{
	public:
		void Exec(const Event* evnt)
		{
			call(evnt);
		}

	protected:
		virtual void call(const Event* evnt) = 0;
	};

	template<class T, class EventType>
	class MethodHandler : public HandlerBase
	{
		typedef void (T::*MemberFunction)(EventType*);

	public:
		MethodHandler(T* instance, MemberFunction method)
			: m_instance(instance), m_method(method)
		{}

		void call(const Event* evnt) override
		{
			(m_instance->*m_method)(static_cast<EventType*>(evnt));
		}

	private:
		T* m_instance;
		MemberFunction m_method;
	};

	class EventDispatcher
	{
		using handler_list = std::list<std::unique_ptr<HandlerBase>>;

	public:
		static EventDispatcher& GetInstace()
		{
			static EventDispatcher instance;
			return instance;
		}

		template<class T, class EventType>
		void Subscribe(T* instance, void (T::*Method)(EventType*))
		{
			std::type_index key = typeid(EventType);

			if (!m_subscribers[key])
			{
				m_subscribers[key] = std::make_unique<handler_list>();
			}

			m_subscribers[key]->push_back(std::make_unique<MethodHandler<T, EventType>>(instance, Method));
		}

		void Fire(const Event* evnt)
		{
			std::type_index key = typeid(*evnt);

			if (!m_subscribers[key])
				return;

			for (auto& handler : *m_subscribers[key])
			{
				handler->Exec(evnt);
			}
		}

	private:
		EventDispatcher() = default;

	private:
		std::map<std::type_index, std::unique_ptr<handler_list>> m_subscribers;
	};
}