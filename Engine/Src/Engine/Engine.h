#pragma once
#include "Core/Object.h"
#include "Core/Timer.h"
#include "EngineDef.h"

namespace Dive
{
	class Context;
	class Event;

	class Engine : public Object
	{
		DIVE_OBJECT(Engine, Object)

	public:
		explicit Engine(Context* pContext);
		~Engine() override;

		bool Initialize(const EngineParameters& parameters);

		void RunFrame();
		void Update();
		void Render();

		bool IsInitialized() const { return m_bInitialized; }
		bool IsExiting() const { return m_bExiting; }

		void Exit();

		void OnExitRequested(const Event& e);

		uint32_t GetMaxFPS() const { return m_MaxFPS; }
		void SetMaxFPS(uint32_t targetFps) { m_MaxFPS = targetFps; }

	private:
		void updateFrameTimer();
		void doExit();
		bool intializeResourceCache();

	private:
		bool m_bInitialized;
		bool m_bExiting;

		Timer m_FrameTimer;

		//std::vector<float> m_LastTmeSteps;

		float m_DeltaTime;

		uint32_t m_MaxFPS;
	};
}

