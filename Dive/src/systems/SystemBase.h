#pragma once

namespace Dive
{
	class World;

	class SystemBase
	{
	public:
		SystemBase(World* world) : m_World(world) {}
		virtual ~SystemBase() = default;

		void SetActiveWorld(World* world) { m_World = world; }

		virtual void OnCreate() {}
		virtual void OnStartRunning() {}
		virtual void OnUpdate() {}
		virtual void OnStopRunning() {}
		virtual void OnDestroy() {}

	protected:
		// static으로 관리해도 될 듯하다.
		World* m_World = nullptr;
	};
}