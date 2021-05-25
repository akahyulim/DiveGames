#pragma once

namespace Editor
{
	class Widget
	{
	public:
		Widget() {}
		virtual ~Widget() {}

		void Begin() {}
		void End() {}
		virtual void Tick() {}
	};
}