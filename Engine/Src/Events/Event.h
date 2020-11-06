
// https://www.gamedev.net/tutorials/_/technical/game-programming/effective-event-handling-in-c-r2459/
// https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f

#pragma once
#include "DivePch.h"

namespace Dive
{
	class IEvent
	{
	public:
		virtual ~IEvent() {}
	};

	class HandlerBase
	{
	public:
		void Exec(const IEvent* evnt)
		{
			call(evnt);
		}

	protected:
		virtual void call(const IEvent* evnt) = 0;
	};

	template<class T, class EventType>
	class MethodHandler : public HandlerBase
	{
		typedef void (T::*MemberFunction)(EventType*);

	public:
		MethodHandler(T* pInstance, MemberFunction method)
			: m_pInstance(pInstance), m_Method(method)
		{}

		void call(const IEvent* evnt) override
		{
			(m_pInstance->*m_Method)(static_cast<EventType*>(evnt));
		}

	private:
		T* m_pInstance;
		MemberFunction m_Method;
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
		void Subscribe(T* pInstance, void (T::*Method)(EventType*))
		{
			if (m_Subscribers[typeid(EventType)] == nullptr)
				m_Subscribers[typeid(EventType)] = std::make_unique<handler_list>();

			m_Subscribers[typeid(EventType)]->push_back(std::make_unique<MethodHandler<T, EventType>>(pInstance, Method));
		}

		void Fire(const IEvent* evnt)
		{
			if (m_Subscribers.empty() || m_Subscribers[typeid(*evnt)] == nullptr)
				return;

			auto itr = m_Subscribers[typeid(*evnt)]->begin();
			auto itr_end = m_Subscribers[typeid(*evnt)]->end();
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
		std::unordered_map<std::type_index, std::unique_ptr<handler_list>> m_Subscribers;
	};
}