#pragma once
#include <memory>

#define DV_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Dive
{
	class Event
	{
	public:
	private:
	private:
	};

	class EventDispatcher
	{
	public:
	private:
	private:
	};
}