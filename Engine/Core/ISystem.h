#pragma once
#include <memory>

namespace Dive
{
	class SystemManager;

	class ISystem
	{
	public:
		ISystem(SystemManager* manager) { m_manager = manager; }
		virtual ~ISystem() = default;

		virtual bool Initialize() { return true; }
		virtual void Update(float deltaTime = 0.0f) {}

	protected:
		SystemManager* m_manager = nullptr;
	};
}