
// https://www.gamedev.net/tutorials/_/technical/game-programming/effective-event-handling-in-c-r2459/
// https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f

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

	class EventSystem
	{
		using handler_list = std::list < std::unique_ptr<HandlerBase>>;
	
	public:
		static EventSystem& GetInstance()
		{
			static EventSystem instance;
			return instance;	
		}

		template<class T, class EventType>
		void Subscribe(T* instance, void (T::*Method)(EventType*))
		{
			if (m_subscribers[typeid(EventType)] == nullptr)
				m_subscribers[typeid(EventType)] = std::make_unique<handler_list>();

			m_subscribers[typeid(EventType)]->push_back(std::make_unique<MethodHandler<T, EventType>>(instance, Method));
		}

		void Fire(const Event* evnt)
		{
			if (m_subscribers.empty() || m_subscribers[typeid(*evnt)] == nullptr)
				return;

			auto itr = m_subscribers[typeid(*evnt)]->begin();
			auto itr_end = m_subscribers[typeid(*evnt)]->end();
			for (itr; itr !=itr_end; itr++)
			{
				(*itr)->Exec(evnt);
			}
		}

	private:
		EventSystem()	= default;
		~EventSystem()	= default;
		EventSystem(const EventSystem&) = delete;

	private:
		std::unordered_map<std::type_index, std::unique_ptr<handler_list>> m_subscribers;
	};
}