#pragma once

namespace Dive
{
	class Context;
	class EventHandler;
	

	class Subsystem
	{
	public:
		explicit Subsystem(Context* context)
			: m_context(context) {}
		virtual ~Subsystem() = default;

		void SubscribeEvent(size_t eventType, EventHandler* handler);
		void UnsubscribeEvent(size_t eventType);

		void SendEvent(size_t eventType);

	protected:
		virtual void onEvent(Subsystem* sender, size_t eventType);

	protected:
		Context* m_context;
		std::list<EventHandler*> m_eventHandlers;
	};

	class EventHandler
	{
	public:
		explicit EventHandler(Subsystem* sender, void* params = nullptr)
			: m_sender(sender), m_eventParams(params) {}
		virtual ~EventHandler() = default;

		// variant type으로 param을 받아야 한다.
		virtual void Invoke() = 0;

		void SetSenderAndEventType(Subsystem* sender, size_t eventType)
		{
			m_sender = sender;
			m_eventType = eventType;
		}

		Subsystem* GetSender()		const { return m_sender; }
		Subsystem* GetReceiver()	const { return m_receiver; }
		size_t GetEventType()		const { return m_eventType; }
		void* GetEventParams()		const { return m_eventParams; }

	protected:
		Subsystem* m_sender;
		Subsystem* m_receiver;
		size_t m_eventType;
		void* m_eventParams;
	};

	// callable + move를 사용하는 편이 낫다.
	// variant를 적용하는 방법을 연구해보자.
	template<class T>
	class EventHandlerImpl : public EventHandler
	{
	public:
		using HandlerFunctionPtr = void(T::*)(size_t);

		EventHandlerImpl(T* receiver, HandlerFunctionPtr function, void* params = nullptr)
			: EventHandler(receiver, params),
			m_function(function)
		{
			assert(m_receiver);
			assert(m_function);
		}

		void Invoke() override
		{
			auto receiver = static_cast<T*>(m_receiver);
			(receiver->*m_function)(m_eventType);
		}

	private:
		HandlerFunctionPtr m_function;
	};
}


#define DIVE_HANDLER(className, function) (new Dive::EventHandlerImpl<className>(this, &className::function))