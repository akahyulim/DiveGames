#pragma once


namespace Dive
{
	class Dive_Context;
	class Dive_EventHandler;

	class Dive_Object
	{
	public:
		Dive_Object(Dive_Context* context);
		virtual ~Dive_Object();

		virtual size_t GetTypeHash()		const = 0;
		virtual std::string GetTypeName()	const = 0;

		void SubscribeEvent(size_t eventType, Dive_EventHandler* handler);
		void UnsubscibeEvent(size_t eventType);

		void SendEvent(size_t eventType);

	protected:
		virtual void onEvent(Dive_Object* sender, size_t eventType);

	protected:
		Dive_Context* m_context;
		std::vector<Dive_EventHandler*> m_eventHandlers;	// 이걸 왜 list로 햇엇을까...
	};

	// template을 사용하지 않은 타입을 object가 가진다.
	// 따라서 base가 필요하다.
	class Dive_EventHandler
	{
	public:
		Dive_EventHandler(Dive_Object* sender, void* userData)
			: m_sender(sender), m_userData(userData) {}
		virtual ~Dive_EventHandler() = default;

		virtual void Invoke() = 0;

		void SetHandler(Dive_Object* sender, size_t eventType)
		{
			m_sender = sender;
			m_eventType = eventType;
		}

		Dive_Object* GetSender() const { return m_sender; }
		Dive_Object* GetReceiver() const { return m_receiver; }	// 이게 왜 필요하냐?
		size_t GetEventType() const { return m_eventType; }
		void* GetUserData() const { return m_userData; }

	protected:
		Dive_Object* m_sender;
		Dive_Object* m_receiver;
		size_t m_eventType;
		// 본래 이름은 user data인데 param과는 관계가 없는듯하다...
		// 스크립트쪽에서 전달한다...
		void* m_userData;
	};

	template<class T>
	class Dive_EventHandlerImpl : public Dive_EventHandler
	{
	public:
		using HandlerFunctionPtr = void(T::*)(size_t);

		Dive_EventHandlerImpl(T* receiver, HandlerFunctionPtr function, void* userData = nullptr)
			: Dive_EventHandler(receiver, userData),
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

	// 이벤트 실행 메소드를 어떻게 등록할지 좀 더 고민해보자.
	template<class T>
	class Dive_EventHandlerImpl11 : public Dive_EventHandler
	{
	public:
		using handlerCallable = std::function<void(T&, size_t)>;

		Dive_EventHandlerImpl11(handlerCallable function, void* userData = nullptr)
			: Dive_EventHandler(nullptr, userData),
			m_function(function)
		{
			assert(m_function);
		}

		void Invoke() override 
		{
			m_function(m_eventType);
		}

	private:
		handlerCallable m_function;
	};
}

#define NEW_DIVE_OBJECT(typeName) \
public: \
size_t GetTypeHash() const override { return typeid(typeName).hash_code(); } \
std::string GetTypeName() const override { return typeid(typeName).name(); } \


#define DIVE_HANDLER(className, function) (new Dive::Dive_EventHandlerImpl<className>(this, &className::function))
// 핸들러 생성 매크로1: 클래스 이름, 메소드 이름
// 핸들러 생성 매크로2: 클래스 이름, 메소드 이름, 파라미터
// event의 정체는 static hash이다. 따라서 파라미터 추가를 염두해둔 매크로 구현이 필요하다.