#pragma once
#include "Core/DvObject.h"
#include "Core/Variant.h"
#include "Core/DvTimer.h"
#include "DvEngineDef.h"

namespace Dive
{
	class DvContext;

	class DvEngine : public DvObject
	{
		DIVE_OBJECT(DvEngine, DvObject)

	public:
		// ������. �⺻ ���� �ý��� ���� �� �ʱ�ȭ.
		explicit DvEngine(DvContext* pContext);
		// �Ҹ���. ��� ���� �ý��� ����.
		~DvEngine() override;

		// ���޹��� �Ķ���ͷ� ���� �ʱ�ȭ �� ������ ���.
		bool Initialize(const EngineParameters& parameters);

		// ������.
		void RunFrame();
		// ������ ������Ʈ �̺�Ʈ �߻�.
		void Update();
		// ������ ������Ʈ ���� ������.
		void Render();

		// ���� �ʱ�ȭ �÷��� ����.
		bool IsInitialized() const { return m_bInitialized; }
		// ���� �÷��� ����
		bool IsExiting() const { return m_bExiting; }

		// �׷��Ƚ� ������ ���� �� ���� ����.
		void Exit();

		// ���� ��û �̺�Ʈ ó��.
		void OnExitRequested();

		unsigned int GetMaxFPS() const { return m_MaxFps; }
		void SetMaxFPS(unsigned int targetFps) { m_MaxFps = targetFps; }

	private:
		// ������ �ð� ���.
		void updateFrameTimer();
		// ���� ���� ����.
		void doExit();

	private:
		// �ʱ�ȭ �÷���.
		bool m_bInitialized;
		// ���� �÷���.
		bool m_bExiting;

		// ������ ���� ���� Ÿ�̸�.
		DvTimer m_FrameTimer;

		//std::vector<float> m_LastTmeSteps;

		// ������ ��� �ð�.
		float m_DeltaTime;

		// �ִ� Ÿ�� ������.
		unsigned int m_MaxFps;
	};
}

