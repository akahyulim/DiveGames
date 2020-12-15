#pragma once
#include "DiveDefs.h"
#include "Object.h"

namespace Dive
{
	class Context;

	// ��� �ð��� Ȯ���ϴ� Ÿ�̸�
	class Timer
	{
	public:
		Timer();

		double GetElapsedTime(bool reset);
		void Reset();

	private:
		std::chrono::high_resolution_clock::time_point m_startTime;
	};

	// app�� time�� ����
	class Time : public Object
	{
		DIVE_OBJECT(Time, Object);

	public:
		Time(Context* context);
		~Time();

		void Update();
		
		double GetElapsedTimeMS()	const { return m_elapsedTimeMS; }
		float GetElapsedTimeSec()	const { return static_cast<float>(m_elapsedTimeMS) / 1000.0f; }
		double GetDeltaTimeMS()		const { return m_deltaTimeMS; }
		float GetDeltaTimeSec()		const { return static_cast<float>(m_deltaTimeMS) / 1000.0f; }

	private:		
		std::chrono::high_resolution_clock::time_point m_startTime;
		std::chrono::high_resolution_clock::time_point m_frameTime;

		double m_deltaTimeMS;
		double m_elapsedTimeMS;
	};
}

