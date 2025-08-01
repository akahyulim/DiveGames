#pragma once

#define DV_EVENT_HANDLER(function)						[this](Dive::EventData data) { function();}
#define DV_EVENT_HANDLER_STATIC(function)				[](Dive::EventData data) { function();}

#define DV_EVENT_HANDLER_DATA(function)					[this](Dive::EventData data) { function(data);}
#define DV_EVENT_HANDLER_DATA_STATIC(function)			[](Dive::EventData data) { function(data);}

#define DV_SUBSCRIBE_EVENT(type, function)				Dive::EventDispatcher::AddListener(type, function)

#define DV_FIRE_EVENT(type)								Dive::EventDispatcher::Dispatch(type)
#define DV_FIRE_EVENT_DATA(type, data)					Dive::EventDispatcher::Dispatch(type, data)

namespace Dive
{
	enum class eEventType
	{
		WindowResized, Exit,

		PreRender, PostRender,

		WorldCreated, WorldModified, 

		Collision, Trigger, KeyPress,
	};

	using EventData = std::variant<int, void*>;

	using EventListener = std::function<void(const EventData&)>;

	class EventDispatcher
	{
	public:
		static void AddListener(eEventType type, EventListener listener);
		static void Dispatch(eEventType type, EventData data = 0);

	private:
		static std::unordered_map<eEventType, std::vector<EventListener>> s_listeners;
	};
}
