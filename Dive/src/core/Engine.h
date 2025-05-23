#pragma once

namespace Dive
{
	class World;

	// Hazel 구조로 보자면 이게 결국 Application이다.
	// 근데 개인적으론 Hazel의 구현이 더 마음에 든다.
	// 문제는 Layer이다.
	class Engine
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void Tick();

		static double GetElapsedTimeMS() { return s_ElapsedTimeMS; }
		static double GetElapsedTimeSec() { return s_ElapsedTimeMS / 1000.0; }
		
		static float GetDeltaTimeMS() { return s_DeltaTimeMS; }
		static float GetDeltaTimeSec() { return s_DeltaTimeMS / 1000.0f; }
		
		static UINT16 GetFps() { return s_Fps; }

		static UINT64 GetFrameCount() { return s_FrameCount; }

		static World* CreateWorld(const std::string& name = "NewWorld");
		static World* GetWorld() { return s_World; }

	private:
		static double s_ElapsedTimeMS;
		static float s_DeltaTimeMS;
		static std::chrono::steady_clock::time_point s_LastTickTime;
		static UINT16 s_Fps;
		static UINT64 s_FrameCount;

		// 일단 멤버 변수로 관리하자.
		static World* s_World;

	};
}
