#pragma once
#include "Core/DvObject.h"

namespace Dive
{
	// timer
	class DvTimer
	{
	public:
		DvTimer();

		double GetMSec(bool bReset);

		void Reset();

	private:
		std::chrono::high_resolution_clock::time_point m_Start;
	};

	class DvContext;

	// time
	class DvTime : public DvObject
	{
		DIVE_OBJECT(DvTime, DvObject)

	public:
		explicit DvTime(DvContext* pContext);
		~DvTime();

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
		DvTimer m_Time;
		// �� ���� �ð� Ÿ�̸�.
		DvTimer m_TimeSinceLevelLoad;

		// ������ ����.
		unsigned int m_FrameCount;

		// ������ ���� �ð�.
		float m_DeltaTime;
	};
}