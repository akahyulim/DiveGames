#pragma once
#include <memory>

namespace Dive
{
	class SystemManager;

	class ISystem
	{
	public:
		ISystem(SystemManager* pManager) { m_pManager = pManager; }
		virtual ~ISystem() = default;

		virtual bool Initialize() { return true; }
		virtual void Update(float deltaTime = 0.0f) {}

	protected:
		SystemManager* m_pManager = nullptr;
	};
}