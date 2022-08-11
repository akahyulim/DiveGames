#pragma once
#include "Core/Object.h"

namespace Dive
{
	// timer
	class Timer
	{
	public:
		Timer();
		~Timer() = default;

		double GetMSec(bool bReset);

		void Reset();

	private:
		std::chrono::high_resolution_clock::time_point m_Start;
	};

	class Context;

	// time
	class Time : public Object
	{
		DIVE_OBJECT(Time, Object)

	public:
		explicit Time(Context* pContext);
		~Time();

		// ������ ����.
		void BeginFrame(float deltaTime);
		// ������ ����.
		void EndFrame();

		// ������ ����.
		unsigned int GetFrameCount() const { return m_FrameCount; }
		// ������ �� ��� �ð�.
		float GetDeltaTime() const { return m_DeltaTime; }

		// �ʴ��� ���α׷� ���� ��� �ð�.
		unsigned int GetTime();
		// �ʴ��� �� ���� ��� �ð�.
		unsigned int GetTimeSinceLevelLoad();

	private:
	private:
		// ���α׷� ���� �ð� Ÿ�̸�.
		Timer m_Time;
		// �� ���� �ð� Ÿ�̸�.
		Timer m_TimeSinceLevelLoad;

		// ������ ����.
		unsigned int m_FrameCount;

		// ������ ���� �ð�.
		float m_DeltaTime;
	};
}