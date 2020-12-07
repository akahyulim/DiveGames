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
		std::vector<Dive_EventHandler*> m_eventHandlers;	// �̰� �� list�� �޾�����...
	};

	// template�� ������� ���� Ÿ���� object�� ������.
	// ���� base�� �ʿ��ϴ�.
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
		Dive_Object* GetReceiver() const { return m_receiver; }	// �̰� �� �ʿ��ϳ�?
		size_t GetEventType() const { return m_eventType; }
		void* GetUserData() const { return m_userData; }

	protected:
		Dive_Object* m_sender;
		Dive_Object* m_receiver;
		size_t m_eventType;
		// ���� �̸��� user data�ε� param���� ���谡 ���µ��ϴ�...
		// ��ũ��Ʈ�ʿ��� �����Ѵ�...
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

	// �̺�Ʈ ���� �޼ҵ带 ��� ������� �� �� ����غ���.
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
// �ڵ鷯 ���� ��ũ��1: Ŭ���� �̸�, �޼ҵ� �̸�
// �ڵ鷯 ���� ��ũ��2: Ŭ���� �̸�, �޼ҵ� �̸�, �Ķ����
// event�� ��ü�� static hash�̴�. ���� �Ķ���� �߰��� �����ص� ��ũ�� ������ �ʿ��ϴ�.