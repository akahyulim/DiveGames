#pragma once

#define EVENT_HANDLER(function)					[this](const Dive::Event& e) { function(); }
#define EVENT_HANDLER_STATIC(function)			[](const Dive::Event& e) { function(); }

#define EVENT_HANDLER_PARAM(function)			[this](const Dive::Event& e) { function(e); }
#define EVENT_HANDLER_PARAM_STATIC(function)	[](const Dive::Event& e) { function(e); }

#define SUBSCRIBE_EVENT(type, function)			Dive::EventHandler::Subscribe(type, function)
#define FIRE_EVENT(e)							Dive::EventHandler::Fire(e)

namespace Dive
{
	// 서브 시스템별 이벤트 타입 열거자.
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

	// 이벤트 타입 함수 매크로.
#define EVENT_CLASS_TYPE(type) \
public:	\
	static eEventType GetStaticType() { return eEventType::type; }	\
	virtual eEventType GetType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

	// 이벤트 타입을 확인하는 기능을 가진 이벤트 베이스 클래스.
	class Event
	{
	public:
		virtual ~Event() = default;

		virtual eEventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
	};

	// 이벤트 구독 Callable.
	using Function = std::function<void(const Event&)>;

	// 이벤트 구독자 관리 및 전달을 수행하는 이벤트 시스템 클래스.
	class EventHandler
	{
	public:
		static void Destroy();
		static void Subscribe(const eEventType eventType, Function&& function);
		static void Fire(const Event& e);
	};
}