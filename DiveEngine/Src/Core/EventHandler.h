#pragma once

#define EVENT_HANDLER(function)					[this](const Dive::Event& e) { function(); }
#define EVENT_HANDLER_STATIC(function)			[](const Dive::Event& e) { function(); }

#define EVENT_HANDLER_PARAM(function)			[this](const Dive::Event& e) { function(e); }
#define EVENT_HANDLER_PARAM_STATIC(function)	[](const Dive::Event& e) { function(e); }

#define SUBSCRIBE_EVENT(type, function)			Dive::EventHandler::Subscribe(type, function)
#define FIRE_EVENT(e)							Dive::EventHandler::Fire(e)

namespace Dive
{
	// ���� �ý��ۺ� �̺�Ʈ Ÿ�� ������.
	enum class eEventType
	{
		// Core
		BeginFrame, PreUpdate, Update, PostUpdate, RenderUpdate, PostRenderUpdate, EndFrame,

		// Graphics
		WindowEvent, ScreenMode, BeginRender, EndRender,

		// Renderer
		RenderTargetUpdate,

		// Scene
		SceneUpdate,

		// IO
		LogMessage, ExitRequested,
	};

	// �̺�Ʈ Ÿ�� �Լ� ��ũ��.
#define EVENT_CLASS_TYPE(type) \
public:	\
	static eEventType GetStaticType() { return eEventType::type; }	\
	virtual eEventType GetType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

	// �̺�Ʈ Ÿ���� Ȯ���ϴ� ����� ���� �̺�Ʈ ���̽� Ŭ����.
	class Event
	{
	public:
		virtual ~Event() = default;

		virtual eEventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
	};

	// �̺�Ʈ ���� Callable.
	using Function = std::function<void(const Event&)>;

	// �̺�Ʈ ������ ���� �� ������ �����ϴ� �̺�Ʈ �ý��� Ŭ����.
	class EventHandler
	{
	public:
		static void Destroy();
		static void Subscribe(const eEventType eventType, Function&& function);
		static void Fire(const Event& e);
	};
}