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

		void ApplyFrameLimit();

	private:
		// ���� ���� ����.
		void doExit();

	private:
		// �ʱ�ȭ �÷���.
		bool m_bInitialized;
		// ���� �÷���.
		bool m_bExiting;

		//DvTimer m_FrameTimer;

		//std::vector<float> m_LastTmeSteps;

		float m_TimeStep;

		//unsigned int m_MinFps;

		//unsigned int m_MaxFps;
	};
}

