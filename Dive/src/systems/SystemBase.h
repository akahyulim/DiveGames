#pragma once

namespace Dive
{
	class DvWorld;

	class SystemBase
	{
	public:
		SystemBase(DvWorld* world) : m_World(world) {}
		virtual ~SystemBase() = default;

		virtual void OnCreate() {}
		virtual void OnStartRunning() {}
		virtual void OnUpdate() {}
		virtual void OnStopRunning() {}
		virtual void OnDestroy() {}

	protected:
		DvWorld* m_World = nullptr;
	};
}